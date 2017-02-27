/*	DAQ test program 
	Date:20070818
	To read analog input value, and then do data processing
	, generate it from analog output channel in a single sampling period

	Acquisition Sampling Rate : 10 kHz
	Output Sampling Rate : 10 kHz

	CH3 / CH4 AI讀取同樣的DATA，看這兩者的值是否不同
	cf = -6
	Amp = 1.32
	Kp = 0.45 ; Ki = 0.15
*/

#include <stdio.h>
#include <windows.h>
#include "NIDAQmx.h"
#include <math.h>
#include "N1231A.h"



//=============================== N1231A ================================== 

N1231A_LOCATION		N1231A_device;
N1231A_HANDLE		hN1231A = NULL;
N1231A_RETURN		N1231A_err;
unsigned short		valid, status;
long				p1, p2, p3, v1, v2, v3;                         // Position & Velocity


//=============================== DAQ Setting ================================== 
inline void			delay(double);
double				servotime;


LARGE_INTEGER	StartTime, CurrentTime, perf_cnt;
TaskHandle      taskAI = 0, taskAO = 0;
int32			DAQ_err;

// 本來在 main 裡面的
int32		read;
int32		err;
bool32		isLate;
double		temp[1];							 // 量回電賀 (SEM 內部使用時指開啟一個 Channel)
double		piezo[2];							 // 輸出電壓


//========= Basic Fixed Parameter ================
const int	    TotalPts = 4000000;                // Total data points, 先切一部分記憶體出來, 實際data有幾個要看  data_points
const int		pre_points = 10;                 // 有些控制器需要上一個時刻的輸出資料，在j=0時為避免出錯，先劃分十個點給"上一個時刻"前的要求
double			y[TotalPts]; 
double			yo[TotalPts];
double			vol[TotalPts];
double			volo[TotalPts];
double			p[TotalPts];
double			vin[TotalPts];
double	        ve[TotalPts];
double			r[TotalPts];                 
double			u[TotalPts];           
double			ey[TotalPts];       
double			ustate[TotalPts];
double			observer[TotalPts];
double			yhat[TotalPts];
double			ax[4];
double			x4[TotalPts];
double			x3[TotalPts];
double			x2[TotalPts];
double			x1[TotalPts];
double			xdot[4];
double			eysum;
int				data_points;                         // Real data points in



//========= Parameter For Debugger ================

int			order;

// (1) Calculation Time Record For Debugger
int			T_clc_dtct = 0;
double		TimeLog[TotalPts];               // store calculating time per data

// (2) Unstable Logger For Debugger
const int   Ustb_Tolerance = 10;                      // 設定不穩定點的容忍度
int			Stability = 1;
int         UstbPts = 0;                               // 一開始0個Unstable point
int         UstbPts_Strog[Ustb_Tolerance];             // 存取不穩定點的編號，最多存1000個不穩定點
// ======= Outer Loop Controller =============
int			Outer_use = 1;		// 是否feedback使用 C(z) (0/1)
// (1) PID
double		Kp = 0.9;		 // SF 0.15;   Ethan 0.45
double		Ki = 0.215;		 //  SF 0.2;    Ethan 0.15
double		Kd = 0;	
// (2) F(z)
double		Fil = 0.8; // Fil = 0 -> no Filter
// FIXED Parameter
double		y_Fil[TotalPts];
double		u_ctrl[TotalPts];   // linear control part of control input
double      u_ctrl_prev;
double		errsum = 0;
double		ediff = 0;
double		e;
// ======= Middle Loop Controller =============
int			Middle_use = 0;		// 是否使用ve fbk (0/1)
double		cf = -6;			// Best choice  cf=-5
double		Amp = 1;			//1.305;
// ======= Inner Loop Or Not ================= * * * *
int          HysCmp = 1;
// ======= Inner Loop A(z) ====================
int		    A_use = 0;		// 是否使用compensation controller (0/1)
// (1) PI
double      Kp_A = 0;
double		Ki_A = 0;		// 0.15
// FIXED Parameter
double      A_cmp[TotalPts];
double		e_cmp_sum = 0;
double		e_cmp;
// ======= Inner Loop Uh_Prev =================
int			 Uh_prv_use = 0;    // 是否使用 Uh_Prev (0/1)
int		     f_ord = 1;         // 使用的filter階數 0 / 1 / 2 (大於2)
// (B) f1(z)
double      f_alpha = 0.8;
// (C) f2(z)
int			N_ord_f1, D_ord_f1;					// 分子 / 分母 階數
double		N_f1_Sum, D_f1_Sum;					// 分子 / 分母 疊代結果
double		N_f1[15], D_f1[15];					// 分子 / 分母 係數 (先劃分15階出來)
// FIXED Parameter
double		Tem = 4.9492;
double		Up[TotalPts], Uh_prev[TotalPts], Vh_cmp[TotalPts];
// ======= Inner Loop Preisach =================
int		    Prsh_use = 0;		// 是否使用 Preisach (0 NULL / 1 變動量 / 2 完整Preisach )
int		    Prsh_include = 1;
int			Pvar_flt_use = 1;
// f(z) for Preisach var
double		f_Prsh_alpha = 0.963;
double      Prsh_var_Amp = 13.6;//1/(1-f_Prsh_alpha);

double      vb = 0;
double      sf = 0;
double		oion = 0;

double          C_left=1;
double			C_right=1;



// FIXED Parameter of Prsh with wipe out
const int   AB_StrgPt = 601;
double		AB[AB_StrgPt];       // ALpha , Beta 一開始先劃分 300組的位置，格式: [ 組數 , [所有Alpha點] , [所有Beta點] , 0 , 0 ... ]
double		Prsh[TotalPts], Prsh_var_Flt[TotalPts],Prsh_Flt[TotalPts];
double		Prsh_cmp;



// Common Parameter
double		Prsh_var;

									
// FIXED Parameter of Prsh WITHOUT wipe out
double			Alpha[300];                          // Assume local maximun would not over than 100 points
double			Beta[300];                           // Assume local minimum would not over than 100 points
int				a_i = 0, b_i = 0;                            // To count the number of Alpha/Beta




// FIXED Parameter of Preisach Poly
double c01;
double c02;
double c03;
double c04;
double c05;
double c10;
double c11;
double c12;
double c13;
double c14;
double c20;
double c21;
double c22;
double c23;
double c30;
double c31;
double c32;
double c40;
double c41;
double c50;


//--------state feedback
double x[4] = { 0, 0, 0, 0, };
double A_s[4][4] = { 0.7924, -0.5774, 0.2122, -0.0644, 0.2617, 0.0316, -0.5790, -0.2128, 0.3741, 0.7966, 0.5345, -0.2200, -0.1918, -0.1427, 0.3338, -0.0996 };
double B_s[4] = { 0.1143, -0.2667, -0.08018, 0.7567 };//column 
double C_s[4] = { 7.1494, -0.8538, 0.2945, 0.1586 };
double K_s[4] = { -0.07, 0.07, 0.07, -0.07 }; //{ -1675.12648141667, 224.309063277069, -901.287931456476, 238.250056623226 };
//{ -0.2234,    0.0907,   -0.1525,    0.0918} { -0.07,   0.07,   0.07,    -0.07 } {-9.8124,    1.3229,   -5.2824,    1.3927}
double L_s[4] = { 0.1830, 0.011, -0.1266, -0.0138 };

//======= ve bias =======
double veerr;

//============================Function Void ========================================

// DAQ
void		Initialization();



// Inner Loop Preisach Model with Wipe out
double		Preisach_WPout(double *u, int j);
void		AB_Login(double uNow, double uPrev);
double		Prsh_intgl(double AB_in[AB_StrgPt]);
double		Fab(double a, double b);    // a = alpha ; b = beta


										// Inner Loop Preisach Model without Wipe out
double Preisach_N0_WPout(double *u, int j);




int main(void)
{
	// --------Basic Parameter----------
	int			i=0, j,k;
	FILE		*fp;

	//-------------------Read Reference Signal--------------
	fp = fopen("Ref_Zig20167_p10_Speed_Lv_28_prezero_0.txt", "r");

	fscanf(fp, "%d", &data_points);
	for (i = pre_points+0; i<pre_points+data_points; i++)
	{
		fscanf(fp, "%lf", &r[i]);
	}
	fclose(fp);


	//-------------------Read Preisach Model-------------------// modified
	fp = fopen("C_coefficient.txt", "r");
	fscanf(fp, "%d", &order);
	if (order == 4){
		fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &c01, &c02, &c03, &c04, &c10, &c11, &c12, &c13, &c20, &c21, &c22, &c30, &c31, &c40);
	}
	if (order == 5){
		fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &c01, &c02, &c03, &c04, &c05, &c10, &c11, &c12, &c13, &c14, &c20, &c21, &c22, &c23, &c30, &c31, &c32, &c40, &c41, &c50);
	}
/*	if (order == 6){
		fscanf(fp, "%lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf %lf", &c01, &c02, &c03, &c04, &c05, &c06, &c10, &c11, &c12, &c13, &c14, &c15, &c20, &c21, &c22, &c23, &c24, &c30, &c31, &c32, &c33, &c40, &c41, &c42, &c50, &c51, &c60 );
	}*/
	fclose(fp);


	//-------------------Read Uh_prev Filter--------------
	if (f_ord == 2){

		fp = fopen("f1_IIR_ord2.txt", "r");

		fscanf(fp, "%d", &N_ord_f1);
		for (i = 0; i<N_ord_f1; i++)
		{
			fscanf(fp, "%lf", &N_f1[i]);
		}

		fscanf(fp, "%d", &D_ord_f1);
		for (i = 0; i<D_ord_f1; i++)
		{
			fscanf(fp, "%lf", &D_f1[i]);
		}
		fclose(fp);
	}

	//--------------Pre Data 歸零--------------------------
	for (i = 0; i<pre_points; i++)
	{
		y[i]=0;
		vol[i]=0;
		yo[i] = 0;
		volo[i] = 0;
		p[i]=0;
		vin[i]=0;
		ve[i]=0;
		r[i]=0;
		u[i]=0;
		TimeLog[i] = 0;
		y_Fil[i]=0;
		u_ctrl[i]=0;  
		A_cmp[i] = 0;
		Up[i]=0;
		Uh_prev[i]=0;
		Vh_cmp[i]=0;
		Prsh[i] = 0;
		Prsh_var_Flt[i] = 0;
		Prsh_Flt[i] = 0;
		ustate[i] = 0;
		observer[i] = 0;
		yhat[i] = 0;
		x4[i] = 0;
		x3[i] = 0;
		x2[i] = 0;
		x1[i] = 0;

	}

	//-------------Start Controller Task-------------------		
	Initialization();
	DAQmxStartTask(taskAO);
	piezo[0] = 0;
	piezo[1] = 0;
	DAQ_err = DAQmxWriteAnalogF64(taskAO, 1, true, 1.0, DAQmx_Val_GroupByChannel, piezo, NULL, NULL);
	delay(1.0);

	DAQmxStartTask(taskAI);
	N1231APresetRaw(hN1231A, 0, 0, &status); //Set axis-1 to Zero 
	N1231APresetRaw(hN1231A, 1, 0, &status); //Set axis-2 to Zero


	//================================================= Whether to Use Hys Cmp
	if (HysCmp == 0){

		Uh_prv_use = 0;
		A_use = 0;
		Prsh_use = 0;
	}
	if (Pvar_flt_use == 0){
		f_Prsh_alpha = 0;
	}
	//-------------------------------------------------Reset Parameter of Preisach Without wipeout
	for (i = 0; i < 1000; i++){
		Alpha[i] = 0;
		Beta[i] = 0;
	}
	//-------------------------------------------------AB must store one basic pair (0,0)
	AB[0] = 1;
	DAQ_err = DAQmxReadAnalogF64(taskAI, -1, 10.0, DAQmx_Val_GroupByScanNumber, temp, 1, &read, NULL);
	veerr = temp[0];
	// ************Control Loop Start from here************
	for (j = pre_points+0; j < pre_points+data_points; j++)
	{

		//=================================================Measure output displacement and capacitance voltage
		if (T_clc_dtct == 1){
			QueryPerformanceCounter(&StartTime);
		}	
		DAQ_err = DAQmxReadAnalogF64(taskAI, -1, 10.0, DAQmx_Val_GroupByScanNumber, temp, 1, &read, NULL);
		ve[j] = temp[0]-veerr*vb;
		N1231AGetRawPosVel(hN1231A, 1, &p2, &v2);
		y[j] = p2 * 0.30908 / 1000;
		vol[j] = v2;
		p[j]=p2;

		N1231AGetRawPosVel(hN1231A, 0, &p1, &v1);
		yo[j] = -p1 * 0.30908 / 1000;
		volo[j] =-v1;
		
		/*
		if (abs(y[j]-y[j-1])>abs(5*(r[j]-r[j-1]))){
			y[j] = y[j-1];
		}
		*/
		//Up[j] = (84 * ve[j] - Tem * y[j - 1]) / 72;	
		Up[j] = (84 * ve[j] - Tem * y[j - 1]) / 36;	
	
		//================================================= Outer Loop Control
		if (Outer_use == 1){
			// (1) y_Fil = F(z) * y 
			y_Fil[j] = Fil*y_Fil[j - 1] + (1 - Fil)*(y[j]);
			//y_Fil[j] = Fil*y_Fil[j - 1] + (1 - Fil)*(2*y[j]-yo[j]);
			//y_Fil[j] = Fil*y_Fil[j - 1] + (1 - Fil)*(yo[j]);
			// (2) e = r - y_Fil
			e = r[j] - y_Fil[j];							// e = r-y
			// (3) u_ctrl = C(z) * e
			errsum += e;	
			// Integrate error
			ediff = (r[j]+r[j-1] - y_Fil[j] - y_Fil[j-1]);//derirator %diff_t=1/5000   =>*50000
			u_ctrl[j] = Kp*e+Ki*errsum+ Kd * ediff ;						// PID control

		}
		else{
			u_ctrl[j] = r[j];
		}
		//================================================= Middle Loop ve feedback
		if (Middle_use == 1){
			u_ctrl[j] = Amp * u_ctrl[j] + cf * ve[j] / 10;
		}
		//=================================================Inner Loop (0) - Uh_Prev
		if (Uh_prv_use == 1){
			// (1) Uh = u - Up - ve*0.1
			Uh_prev[j] = u[j - 1] - Up[j] - ve[j] / 10;
			// (2) Vh_cmp = filter(z) * Uh_prev --- f_ord 三個選項 -- (A)0階 / (B)1階 / (C)2階
			// (A) Vh_cmp =  Uh_prev 不通過 filter
			if (f_ord == 0){
				Vh_cmp[j] = Uh_prev[j];
			}
			// (B) Vh_cmp = f1(z) * Uh_prev   通過1階 filter
			else if (f_ord == 1){
				Vh_cmp[j] = (f_alpha)*Vh_cmp[j - 1] + (1 - f_alpha)*Uh_prev[j];
			}
			// (C) Vh_cmp = f2(z) * Uh_prev   通過2階 filter
			else if (f_ord > 1){
				N_f1_Sum = 0;
				for (i = 0; i < N_ord_f1; i++)	// N_F_Sum = ( N0 + N1 * z^-1 + N2 * z^-2 + ... ) *  y[j] 
				{
					if (j - i < 0) break;
					N_f1_Sum += N_f1[i] * Uh_prev[j - i];
				}
				D_f1_Sum = 0;
				for (i = 0; i < D_ord_f1; i++)	// D_F_Sum = (     D0 * z^-1 + D1 * z^-2 + ... ) *  y_Fil[j] 
				{
					if (j - i <= 0) break;
					D_f1_Sum += D_f1[i] * Vh_cmp[j - i - 1];
				}
				Vh_cmp[j] = N_f1_Sum + D_f1_Sum;
			}
		}
		//=================================================Inner Loop (1) - Compensation Controller A(z)
		if (A_use == 1) {		
		// (1) u_ctrl in Previous time
		u_ctrl_prev = u_ctrl[j - 1];
		// (2) Calculate Compensation Error
		e_cmp = u_ctrl_prev - Up[j] - ve[j] / 10;
		// (3) A_cmp = A(z) * e_cmp
		e_cmp_sum += e_cmp;
		A_cmp[j] = (Ki_A * e_cmp_sum + Kp_A * e_cmp);
		}
		//-------------------------------------------- state feedback
		for (k = 0; k < 4; k++)
		{
			yhat[j] += C_s[k] * x[k];
		}
		ey[j] = y[j] - yhat[j];
		eysum += ey[j];
		for (i = 0; i < 4; i++)
		{
			ax[i] = 0;
			for (k = 0; k < 4; k++)
			{
				ax[i] += A_s[i][k] * x[k];
			}
		}
		for (i = 0; i < 4; i++)
		{
			ustate[j] += (-K_s[i] * x[i]);
		}
		for (i = 0; i<4; i++)
		{
			xdot[i] = B_s[i] * (u_ctrl[j] + ustate[j]) + ax[i] + L_s[i] * (y[j] - yhat[j]) + oion*0.001*L_s[i] * eysum;
		}
		for (i = 0; i < 4; i++)
		{
			x[i] = xdot[i];
		}
		observer[j] = yhat[j];
		x1[j] = x[0];
		x2[j] = x[1];
		x3[j] = x[2];
		x4[j] = x[3];

		//*** =================   u   ========================== ***
		u[j] = u_ctrl[j] + Vh_cmp[j] + A_cmp[j] + Prsh_cmp + sf*ustate[j];
		//=================================================Inner Loop (2) - Preisach Model
		if (Prsh_use == 1){                 // Preisach Var 補償
			// (1) Preisach Model
			Prsh[j] = Preisach_WPout(u, j);
			Prsh_var = Prsh[j] - Prsh[j - 1];
			// Prsh[j] = Preisach_N0_WPout(u, j);
			// Prsh[j] = Preisach_WPout(u, j);

			// (2) Filter
			if (Pvar_flt_use == 1){
				Prsh_var_Flt[j] = (f_Prsh_alpha)*Prsh_var_Flt[j - 1] + (1 - f_Prsh_alpha)*Prsh_var;
				Prsh_cmp = Prsh_var_Flt[j];
			}
			else{
				Prsh_cmp = Prsh_var;
			}
			// (3) Preisach_var 放大倍率
			Prsh_cmp = Prsh_var_Amp * Prsh_cmp;
		}
		else if (Prsh_use == 2) {			// 完整 Preisach 補償
			// (1) Preisach Model
			Prsh[j] = Preisach_WPout(u, j);
			// (2) Compensate
			Prsh_Flt[j] = (f_Prsh_alpha)*Prsh_Flt[j - 1] + (1 - f_Prsh_alpha)*Prsh[j];
			Prsh_cmp = Prsh[j];
		}		
		//================================================= Control Signal Feed to System=================================================
		if (Prsh_include == 1){
			//u[j] = u[j] + Prsh_cmp;
			piezo[0] = u[j];
			piezo[1] = u[j];
		}
		else{
			piezo[0] = u[j] + Prsh_cmp;
			piezo[1] = u[j] + Prsh_cmp;
		}		
		if (HysCmp == 0){
			piezo[0] = u_ctrl[j] ;
			piezo[1] = u_ctrl[j] ;
			u[j] = u_ctrl[j];
		}
		if (u[j]>=7.5) { 
			piezo[0] = 7.5;
			piezo[1] = 7.5;
			UstbPts_Strog[UstbPts] = j ;
			UstbPts += 1;
		}
		else if (u[j]<=-1) {

			piezo[0] = -1;
			piezo[1] = -1;
			UstbPts_Strog[UstbPts] = j ;
			UstbPts += 1;
		}
		
		if		(UstbPts >= Ustb_Tolerance ) {

			    Stability = 0 ;
				break; // upper saturation for protection
		}

		vin[j] = piezo[0];
		piezo[0]=piezo[0]*C_left;
		piezo[1]=piezo[1]*C_right;
		DAQmxWriteAnalogF64(taskAO, 1, 1, 10.0, DAQmx_Val_GroupByChannel, piezo, NULL, NULL);

		err = DAQmxWaitForNextSampleClock(taskAI, 0.1, &isLate);

		QueryPerformanceCounter(&CurrentTime);

		if (T_clc_dtct == 1){

			TimeLog[j] = ((double)CurrentTime.QuadPart-(double)StartTime.QuadPart)/perf_cnt.QuadPart;		   
		}		   
	}		
	piezo[0] = 0;
	piezo[1] = 0;
	DAQmxWriteAnalogF64(taskAO, 1, 1, 10.0, DAQmx_Val_GroupByChannel, piezo, NULL, NULL);
	delay(0.5);
	
	DAQmxStopTask(taskAI);
	DAQmxClearTask(taskAI);
	DAQmxStopTask(taskAO);
	DAQmxClearTask(taskAO);

	//	===== Save Data =====

	fp = fopen("Measurement_Data.txt","w");
	for (i= pre_points+0 ; i<pre_points+data_points ; i++)
	{
		fprintf(fp,"%f %f %f %f %f %f %f %f %f\n", y[i], u[i], ve[i], u_ctrl[i], vol[i],yo[i], volo[i],p[i],vin[i]);
	}
	fclose(fp);	
	

	fp = fopen("Module_Usage.txt", "w");
	fprintf(fp, "%d %d %d %d %d \n", Outer_use, Middle_use, Uh_prv_use, A_use, Prsh_use);
	fclose(fp);


	if (Outer_use == 1){

		fp = fopen("OuterLoop_Setting.txt", "w");
		fprintf(fp, "%f %f %f %f \n", Kp, Ki, Kd,Fil);
		fclose(fp);


		fp = fopen("OuterLoop_Results.txt", "w");
		for (i = pre_points+0; i<pre_points+data_points; i++)
		{
			fprintf(fp, "%f %f \n", y_Fil[i], u_ctrl[i]);
		}
		fclose(fp);
	}


	if (Middle_use == 1){

		fp = fopen("MiddleLoop_Setting.txt", "w");
		fprintf(fp, "%f %f \n", cf, Amp);
		fclose(fp);
	
		if (Outer_use == 0){

			fp = fopen("MiddleLoop_Results.txt", "w");
			for (i = pre_points+0; i<pre_points+data_points; i++)
			{
				fprintf(fp, "%f \n", u_ctrl[i]);
			}
			fclose(fp);
		}
	}



	if (Uh_prv_use == 1){

		fp = fopen("Uh_prev_Setting.txt", "w");
		fprintf(fp, "%d %f \n", f_ord, f_alpha);
		fclose(fp);

		fp = fopen("Uh_prv_Results.txt", "w");
		for (i = pre_points+0; i<pre_points+data_points; i++)
		{
			fprintf(fp, "%f %f \n", Uh_prev[i], Vh_cmp[i]);
		}
		fclose(fp);
	}

	if (A_use == 1){

		fp = fopen("A_Setting.txt", "w");
		fprintf(fp, "%f %f \n", Kp_A, Ki_A);
		fclose(fp);

		fp = fopen("A_Results.txt", "w");
		for (i = pre_points+0; i<pre_points+data_points; i++)
		{
			fprintf(fp, "%f \n", A_cmp[i]);
		}
		fclose(fp);
	}

	if (Prsh_use == 1){

		fp = fopen("Prsh_Setting.txt", "w");
		fprintf(fp, "%d %d %f %f \n", Prsh_include, Pvar_flt_use, f_Prsh_alpha, Prsh_var_Amp);
		fclose(fp);


		if (Pvar_flt_use == 1){

			fp = fopen("Prsh_Results.txt", "w");
			for (i = pre_points+0; i<pre_points+data_points; i++)
			{
				fprintf(fp, "%f %f \n", Prsh[i], Prsh_var_Flt[i]);
			}
			fclose(fp);
		}
		else{

			fp = fopen("Prsh_Results.txt", "w");
			for (i = pre_points+0; i<pre_points+data_points; i++)
			{
				fprintf(fp, "%f \n", Prsh[i]);
			}
			fclose(fp);
		}
		
	}
	else if (Prsh_use == 2) {
		fp = fopen("Prsh_Results.txt", "w");
		for (i = pre_points + 0; i<pre_points + data_points; i++)
		{
			fprintf(fp, "%f \n", Prsh[i]);
		}
		fclose(fp);
		if (Pvar_flt_use == 1){

			fp = fopen("Prsh_Results.txt", "w");
			for (i = pre_points+0; i<pre_points+data_points; i++)
			{
				fprintf(fp, "%f %f \n", Prsh[i], Prsh_var_Flt[i]);
			}
			fclose(fp);
		}
		else{

			fp = fopen("Prsh_Results.txt", "w");
			for (i = pre_points+0; i<pre_points+data_points; i++)
			{
				fprintf(fp, "%f \n", Prsh[i]);
			}
			fclose(fp);
		}
		
	}

	

	fp = fopen("Stability_and_TimeClc.txt", "w");
	fprintf(fp, "%d %d \n", Stability, T_clc_dtct);
	fclose(fp);

	

	if (UstbPts >= Ustb_Tolerance){

		fp = fopen("Ustb_Pts_Log.txt", "w");
		for (i = 0; i<UstbPts; i++)
		{
			fprintf(fp, "%d \n", UstbPts_Strog[i]);
		}
		fclose(fp);
	}


	if (T_clc_dtct == 1){

		fp = fopen("Time_Log.txt","w");
		for (i= pre_points+0 ; i<pre_points+data_points ; i++)
		{
			fprintf(fp,"%f \n", TimeLog[i]); 
		}
		fclose(fp);	
	}


	return 0;

}
inline void delay(double delay_time)
{
	QueryPerformanceCounter(&StartTime);
	delay_time = delay_time * perf_cnt.QuadPart;

	do {QueryPerformanceCounter(&CurrentTime);}
	while ( ((double)CurrentTime.QuadPart-(double)StartTime.QuadPart) < delay_time );
}

//===============================Initial====================================
void Initialization()
{
	// int		PZTservoON[3] = {TRUE, TRUE, TRUE}; 2015/09/07
	// int		*check=new int[3];					2015/09/07
	unsigned short	status = 0;

	QueryPerformanceFrequency(&perf_cnt);         // CPU Frequency=3579545;
	printf("System frequency : %d\n",perf_cnt.QuadPart);
	servotime = perf_cnt.QuadPart / 5000;         // 每次sampling 可以分到的CPU clock數目
	
	SetPriorityClass(GetCurrentProcess(), REALTIME_PRIORITY_CLASS);
	SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_TIME_CRITICAL);

	// Input Task Configuration
	DAQmxCreateTask("", &taskAI);
	DAQmxCreateAIVoltageChan(taskAI, "Dev1/ai3 ", "", DAQmx_Val_Cfg_Default, -10.0, 10.0, DAQmx_Val_Volts, NULL);
	DAQmxCfgSampClkTiming(taskAI, "", 5000.0, DAQmx_Val_Rising, DAQmx_Val_HWTimedSinglePoint, 0);
	DAQmxSetRealTimeWaitForNextSampClkWaitMode(taskAI, DAQmx_Val_Poll);        //    Fastest possible way to check if sample clock arrives

	// Output Task Configuration
	DAQmxCreateTask("", &taskAO);
	DAQmxCreateAOVoltageChan(taskAO, "Dev1/ao1, Dev1/ao3", "", -10.0, 10.0, DAQmx_Val_Volts, NULL);
	DAQmxCfgSampClkTiming(taskAO, "", 5000.0, DAQmx_Val_Rising, DAQmx_Val_HWTimedSinglePoint, 0);


//	=====N1231A axis board=====
	N1231ADefaultDevice(&N1231A_device);                         
	N1231A_err = N1231AOpen(&N1231A_device, &hN1231A, NULL);
}



// =================Preisach model with wipe out # Main Code=================
double Preisach_WPout(double *u, int j)
{
	double	Bias = 0;
	double	Prsh_result;


	AB_Login(u[j], u[j - 1]);                          // AB更新
	Prsh_result = Prsh_intgl(AB) + Bias;					// 現在的Preisach			     	


	return Prsh_result;
}

// 計算當下的Alpha / Beta
void AB_Login(double uNow, double uPrev)
{
	//------------------------------------------基本設定
	// 輸入值為 Global 變數 AB
	int		OptCdn;                             // Operating Condition with initial value = 0
	int		AB_ln0;							   // 輸入的 Alpha / Beta 數目
	int		delete_ln;
	int		Tsln;
	int		i;
	double	AB_Org[AB_StrgPt];


	AB_ln0 = AB[0];								   // Alpha original 長度 = array AB 第一個存的值 
	delete_ln = 0;
	OptCdn = 0;                                        // 1 Rising ; -1 Descending ; 0 Maintain

	for (i = 0; i <= 2 * AB_ln0; i++)           // 另一個叫做AB_org的矩陣把AB存住，再把AB更新
	{
		AB_Org[i] = AB[i];
	}


	if (uNow > uPrev){

		OptCdn = 1;                                // 當下的操作情形: 上升 = 1 / 下降 = -1
	}
	else if (uNow < uPrev){

		OptCdn = -1;							   // 當下的操作情形: 上升 = 1 / 下降 = -1
	}
		//------------------------------------------Rising
	if (OptCdn == 1){

		for (i = 1; i <= AB_ln0 - 1; i++)			  // 計算要Wipe Out的點
		{
			if (uNow >= AB_Org[AB_ln0 + 1 - i])	      // Check from point [0 + AB_ln0] to [0+1 + 1] 是因為 (1) AB[0]=alpha/beta存的長度 ; (2) AB[1]= 最基本的(a, b) 點(0, 0) 必定不可消去 
			{
				delete_ln = delete_ln + 1;				  // delete_ln 為要刪除的資料點
			}
			else
			{
				break;
			}
		}
		Tsln = AB_ln0 - delete_ln;					 // Alpha 被wipe out後暫時剩下的點數目(新的點還沒登錄上去，只有把舊的點wipe out)
	}

	// ------------------------------------------Descending
	else if (OptCdn == -1)
	{
		for (i = 1; i <= AB_ln0 - 1; i++)				// 計算要Wipe Out的點
		{
			if (uNow < AB_Org[AB_ln0 + AB_ln0 + 1 - i])	// Check from point [(0+AB_ln0) + AB_ln0] to [(0+AB_ln0) + 2] 是因為 (1) AB[(0+AB_ln0) + 1] 到 [(0+AB_ln0) + AB_ln0]=存的才是 beta; (2) AB[(0+AB_ln0) + 1]= 最基本的(a, b) 點(0, 0) 必定不可消去  
			{
				delete_ln = delete_ln + 1;				// delete_ln 為要刪除的資料點
			}
			else
			{
				break;
			}
		}
		Tsln = AB_ln0 - delete_ln;					// Transcient Length(新的點還沒登錄上去，只有把舊的點wipe out)
	}
	//------------------------------------------把目前的u登錄到Alpha / Beta
	if (OptCdn == 1)							// Rising
	{
		AB[0] = Tsln + 1;					// 輸出的Alpha Beta 長度，第一個值是 alpha/beta length
		for (i = 1; i <= Tsln; i++)			// Remained Alpha / Beta storage
		{
			AB[i] = AB_Org[i];
			AB[Tsln + 1 + i] = AB_Org[AB_ln0 + i];  // Tsln + 1 = 新的 Alpha / Beta 長度
		}
		AB[Tsln + 1] = uNow;
		AB[2 * (Tsln + 1)] = uNow;
	}
	else if (OptCdn == -1)                       // Decreasing
	{
		AB[0] = Tsln + 1;					// 輸出的Alpha Beta 長度，第一個值是 alpha/beta length
		for (i = 1; i <= Tsln; i++)			// Remained Alpha / Beta storage
		{
			AB[i] = AB_Org[i];
			AB[Tsln + 1 + i] = AB_Org[AB_ln0 + i];
		}
		AB[Tsln + 1] = AB_Org[Tsln + 1];
		AB[2 * (Tsln + 1)] = uNow;
	}
	else                                         // Maintain
	{
	}
}
// 計算 Preisach 積分面積
double Prsh_intgl(double AB_in[AB_StrgPt])
{
	double a, b, P_ab;
	int	   i, Alpha_ln;
	double Tri_Integral_1, Tri_Integral_2, Correction_term;
	double Trpz_Integral;
	double Prsh_Result;

	Alpha_ln = AB_in[0];
	Tri_Integral_1 = 0;
	Tri_Integral_2 = 0;
	Trpz_Integral = 0;
	Prsh_Result = 0;

	for (i = 2; i <= Alpha_ln; i++)			  // 計算要Wipe Out的點
	{
		// ------------------------ - 計算大的三角形
		a = AB_in[i];
		b = AB_in[Alpha_ln + i - 1];

		P_ab = Fab(a, b);
		Correction_term = Fab(a, a);

		Tri_Integral_1 = P_ab - Correction_term;
		// ------------------------ - 計算減去小三角形變成梯形
		b = AB_in[Alpha_ln + i];
		P_ab = Fab(a, b);
		Correction_term = Fab(a, a);
		Tri_Integral_2 = P_ab - Correction_term;

		Trpz_Integral = Tri_Integral_1 - Tri_Integral_2;

		// ------------------------ - 所有梯形面疊加

		Prsh_Result = Prsh_Result + Trpz_Integral;
	}

	return Prsh_Result;
}


// 計算 F(a,b)
double Fab(double a, double b)    // a = alpha ; b = beta
{
	if (order == 4) {
		return (c10*a + c01*b) + (c11*a*b + c20*pow(a, 2) + c02*pow(b, 2)) + (c21*pow(a, 2)*b + c12*a*pow(b, 2) + c30*pow(a, 3) + c03*pow(b, 3)) + (c13*a*pow(b, 3) + c31*pow(a, 3)*b + c22*pow(a, 2)*pow(b, 2) + c40*pow(a, 4) + c04*pow(b, 4));
	}
	if (order == 5){
		return(c10*a + c01*b) + (c11*a*b + c20*pow(a, 2) + c02*pow(b, 2)) + (c21*pow(a, 2)*b + c12*a*pow(b, 2) + c30*pow(a, 3) + c03*pow(b, 3)) + (c13*a*pow(b, 3) + c31*pow(a, 3)*b + c22*pow(a, 2)*pow(b, 2) + c40*pow(a, 4) + c04*pow(b, 4)) + (c50*pow(a, 5) + c14*a*pow(b, 4) + c41*pow(a, 4)*b + c23*pow(a, 2)*pow(b, 3) + c32*pow(a, 3)*pow(b, 2) + c05*pow(b, 5));
	}
	/*if (order == 6){
		return(c10*a + c01*b) + (c11*a*b + c20*pow(a, 2) + c02*pow(b, 2)) + (c21*pow(a, 2)*b + c12*a*pow(b, 2) + c30*pow(a, 3) + c03*pow(b, 3)) + (c13*a*pow(b, 3) + c31*pow(a, 3)*b + c22*pow(a, 2)*pow(b, 2) + c40*pow(a, 4) + c04*pow(b, 4)) + (c50*pow(a, 5) + c14*a*pow(b, 4) + c41*pow(a, 4)*b + c23*pow(a, 2)*pow(b, 3) + c32*pow(a, 3)*pow(b, 2) + c05*pow(b, 5)) + (c60*pow(a, 6) + c06*pow(b, 6) + c15*a*pow(b, 5) + c51*pow(a, 5)*b + c24*pow(a, 2)*pow(b, 4) + c42*pow(a, 4)*pow(b, 2) + c33*pow(a, 3)*pow(b, 3));
	}*/
}

// =================Preisach model without wipe out # Main Code=================
double Preisach_N0_WPout(double *u, int j)
{
	double Bias = 0;
	double Prsh_Nw;
	static double Prsh_Pv, Fab_past;
	static int	  type;

	if (j == 0) { 
		Fab_past = 0; 
		type = 0;
		Prsh_Pv = 0;
	}

	// (1) Part 1 ------------------------------------Ascending
	if (r[j] > r[j - 1])
	{

		//----Calculate Hysteresis
		type = 1;
		Alpha[a_i] = u[j];
		Prsh_Nw = Fab_past + (Fab(Alpha[a_i], Beta[b_i]) - Fab(Alpha[a_i], Alpha[a_i]));


		//----Calculate Fab_past
		if (r[j] > r[j + 1])
		{
			Fab_past = Prsh_Nw;
			b_i = b_i + 1;
		}
	}

	// (1) Part 2 ------------------------------------Decending 
	else if (r[j] < r[j - 1])
	{

		//----Calculate Hysteresis
		type = -1;
		Beta[b_i] = u[j];
		Prsh_Nw = Fab_past - (Fab(Alpha[a_i], Beta[b_i]) - Fab(Alpha[a_i], Alpha[a_i]));


		//----Calculate Fab_past
		if (r[j] < r[j + 1])
		{
			Fab_past = Prsh_Nw;
			a_i = a_i + 1;
		}
	}


	// (1) Part 3 ------------------------------------Maintain
	else if (r[j] == r[j - 1])
	{
		if (type == 1)
		{
			//----Calculate Hysteresis
			Alpha[a_i] = u[j];
			Prsh_Nw = Fab_past + (Fab(Alpha[a_i], Beta[b_i]) - Fab(Alpha[a_i], Alpha[a_i]));


			//----Calculate Fab_past
			if (r[j] > r[j + 1])
			{
				Fab_past = Prsh_Nw;
				b_i = b_i + 1;
			}
		}

		else if (type == -1)
		{
			//----Calculate Hysteresis
			Beta[b_i] = u[j];
			Prsh_Nw = Fab_past - (Fab(Alpha[a_i], Beta[b_i]) - Fab(Alpha[a_i], Alpha[a_i]));


			//----Calculate Fab_past
			if (r[j] < r[j + 1])
			{
				Fab_past = Prsh_Nw;
				a_i = a_i + 1;
			}
		}

		else if (type == 0)
		{
			Prsh_Nw = 0;
		}
	}

	// (1) Part 5 ------------------------------------END Preisach
	Prsh_Nw = Prsh_Nw + Bias;


	Prsh_Pv = Prsh_Nw;
	return Prsh_Nw;
}


