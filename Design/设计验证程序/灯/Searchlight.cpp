#include <stdio.h>
#include <math.h>

#define uint8_t char

#define PI 3.141592



class Light {
private:
    
    

public:
    Light();
    ~Light();
    float Get_Phi();                    //获取当前的Phi角	*
    float Get_Sigma();                  //获取当前的Sigma角	*
    void SetGround(float h);            //设置灯离地面的高	*
    void SetWall(float l);            //设置灯离地面的高	*
    void SetObServer(float x,float y, float z);     //设置人的相对灯的位置	*
    void Init(float h,float x,float y, float z, float l);    //直接设置 灯离地面的高 与 人的相对灯的位置  与墙面的位置 *
    uint8_t AutoSetGround(float alpha_1,float beta_1,float phi_1 ,float sigma_1,float alpha_2,float beta_2,float phi_2 ,float sigma_2);           //通过多次校准获取人和地面相对于灯塔的位置 *
	uint8_t AutoSetWall(float alpha_1,float beta_1,float phi_1 ,float sigma_1);           //通过多次校准获取灯相对于墙面的距离 *
    void Flush(float alpha, float beta);            //输入 人的观察角 并更新 灯的照射角 *
    
    
    float Phi,Sigma;    //灯照射的角度
    uint8_t HaveNone;	//错误返回值 0 无错误 1 有错误
    float H = 100;            //灯高
    float X = 0,Y = 0;        //人相对于灯的位置
    float Z = 100 ;         //人相对于地面的高
    float L = 0;			//墙的位置
    float Angle_to_Rad(float angle);    //角度转弧度	*
    float Rad_to_Angle(float rad);      //弧度转角度    *
     //平面与直线的交点 如果返回的数组turnback长度是4则无法求得交点 planeVevtor 平面的法线向量,平面上的点，直线方向向量，直线上的点	*
    void  CalPlaneLineIntersectPoint(float* turnback,float* planeVector, float* planePoint, float* lineVector, float* linePoint);	 
    void  GetTheLine(float* turnback,float x,float y, float z,float phi,float sigma);    //通过定点和alpha，sigma返回直线对象 	*
    void  GetThePlane(float* turnback,float x,float y, float z,float phi,float sigma);    //通过定点和alpha，sigma返回面对象	*
    void  GetTheAngle(float* turnback,float* a , float* b );   // 通过两个定点确定a观测b时的观测角 *
    
    void Turn();                                    //旋转灯塔
    void Turn(float phi, float sigma);              //旋转灯塔
};

Light::Light(){
}

Light::~Light(){

}

void Light :: SetGround(float h){
    H = h;
}

void Light :: SetWall(float l){
	L = l;
}

void Light :: SetObServer(float x,float y,float z){
    X = x;
    Y = y;
    Z = H - z;
}

void Light ::  Init(float h,float x,float y, float z , float l ){
    SetGround(h);
    SetObServer(x,y,z);
    SetWall(l);
}

void Light ::  Flush(float alpha, float beta){
	
    //构建地面
    float ground[6];
    GetThePlane(ground,0,0,0,0,0);
	//求点C
	float c[3];
	float bc[6];
	GetTheLine(bc,X,Y,Z,alpha,beta);
	CalPlaneLineIntersectPoint(c,&ground[3],&ground[0],&bc[3],&bc[0]);
	//判断C是否在墙上
	if(  (c[1] > L) && (L!=0) ){
		//构建墙面
		float wall[6];
		GetThePlane(wall,0,L,0,90,90);
		//将C点改到墙面上
		CalPlaneLineIntersectPoint(c,&wall[3],&wall[0],&bc[3],&bc[0]);
	}
	for(int i = 0 ;i < 3 ;i++){
		printf("C: %f\n",c[i]);
	}
	//求出AC观测角
	float a[3] = {0,0,H};
	float angle[2];
	GetTheAngle(angle,a,c);
	
	if( alpha >= 90 && L == 0 ){
		angle[0] = alpha;
		angle[1] = beta;
	} 
	
	
	Phi = angle[0];
	Sigma = angle[1];
	
	

}

float Light ::  Get_Phi(){
    return Phi;
}

float Light ::  Get_Sigma(){
    return Sigma;
}

void Light ::  Turn(){
    printf("Sigma:%f",Sigma);
    printf("Phi:%f",Phi);
}

float Light ::  Angle_to_Rad(float angle){
    return angle * PI / 180;
}

float Light ::  Rad_to_Angle(float rad){
    return rad * 180 / PI;
}

void Light :: CalPlaneLineIntersectPoint(float* turnback, float* planeVector, float* planePoint, float* lineVector, float* linePoint){
    float vp1, vp2, vp3, n1, n2, n3, v1, v2, v3, m1, m2, m3, t,vpt;
    vp1 = planeVector[0];
    vp2 = planeVector[1];
    vp3 = planeVector[2];
    n1 = planePoint[0];
    n2 = planePoint[1];
    n3 = planePoint[2];
    v1 = lineVector[0];
    v2 = lineVector[1];
    v3 = lineVector[2];
    m1 = linePoint[0];
    m2 = linePoint[1];
    m3 = linePoint[2];
    vpt = v1 * vp1 + v2 * vp2 + v3 * vp3;
    //首先判断直线是否与平面平行
    //如果平行则返回一个长度为1大小为4的数组
    if (vpt == 0){
    	HaveNone = 1;
    }
    else{
        t = ((n1 - m1) * vp1 + (n2 - m2) * vp2 + (n3 - m3) * vp3) / vpt;
        turnback[0] = m1 + v1 * t;
        turnback[1] = m2 + v2 * t;
        turnback[2] = m3 + v3 * t;
        HaveNone = 0;
    }
}

uint8_t Light :: AutoSetGround(float alpha_1,float beta_1,float phi_1 ,float sigma_1,float alpha_2,float beta_2,float phi_2 ,float sigma_2){
	//固定地面
	H = 100;
	float ground[6];
	GetThePlane(ground,0,0,0,0,0);
	/*
	for(int i=0 ; i<6 ;i++){
		printf("G:%f\n",ground[i]);
	}
	*/
	//求C
	float c[3];
	float lineac[6];
	GetTheLine(lineac,0,0,H,phi_1,sigma_1);
	CalPlaneLineIntersectPoint(c,&ground[3],&ground[0],&lineac[3],&lineac[0]);	
	/*
	for(int i=0 ; i<6 ;i++){
			printf("lineAC:%f\n",lineac[i]);
	}
	for(int i=0 ; i<3 ;i++){
			printf("C:%f\n",c[i]);
	}
	*/
	//求D
	float d[3];
	float linead[6];
	GetTheLine(linead,0,0,100,phi_2,sigma_2);
	CalPlaneLineIntersectPoint(d,&ground[3],&ground[0],&linead[3],&linead[0]);
	/*
	for(int i=0 ; i<6 ;i++){
			printf("LineAD:%f\n",linead[i]);
	}
	for(int i=0 ; i<3 ;i++){
			printf("D:%f\n",d[i]);
	}
	*/
	//求CB
	float linecb[6];
	GetTheLine(linecb,c[0],c[1],c[2],alpha_1,beta_1);
	/*
	for(int i=0 ; i<6 ;i++){
			printf("LineCB:%f\n",linecb[i]);
	}
	*/
	//求垂直于地面的面DB
	float planedb[6];
	alpha_2 = 90;
	GetThePlane(planedb,d[0],d[1],d[2],alpha_2,beta_2+90);
	/*
	for(int i=0 ; i<6 ;i++){
			printf("planeDB:%f\n",planedb[i]);
	}
	*/
	//求出点B
	float b[3];
	CalPlaneLineIntersectPoint(b,&planedb[3],&planedb[0],&linecb[3],&linecb[0]);
	/*
	for(int i=0 ; i<3 ;i++){
			printf("B:%f\n",b[i]);
	}
	*/
	if( HaveNone == 0){
		X = b[0];
		Y = b[1];
		Z = b[2];
		return 1;
	}else {
		return 0;
	}
	
	
	
}

uint8_t Light :: AutoSetWall(float alpha_1,float beta_1,float phi_1 ,float sigma_1){         //通过多次校准获取灯相对于墙面的距离
	float c[3]; 
	float planeac[6];
	float linebc[6];
	phi_1 = 90 ;
	GetThePlane(planeac,0,0,H,phi_1,sigma_1+90);
	GetTheLine(linebc,X,Y,Z,alpha_1,beta_1);
	/*
	for(int i=0 ; i<6 ;i++){
			printf("LineBD:%f\n",linebc[i]);
	}
	for(int i=0 ; i<6 ;i++){
			printf("PlaneAC:%f\n",planeac[i]);
	}
	*/	
	CalPlaneLineIntersectPoint(c,&planeac[3],&planeac[0],&linebc[3],&linebc[0]);
	if( HaveNone == 0){
		L = c[1];
		/*
		for(int i=0 ; i<3 ;i++){
				printf("C:%f\n",c[i]);
		}
		*/
		return 1;
	}else {
		return 0;
	}
}

void Light :: GetTheLine(float* turnback,float x,float y, float z,float phi,float sigma){
 	phi = Angle_to_Rad(180 - phi);
 	sigma = Angle_to_Rad(sigma);
    turnback[0] = x;
	turnback[1] = y;
	turnback[2] = z;
	turnback[3] = 10 * sin(phi) * cos(sigma);
	turnback[4] = 10 * sin(phi) * sin(sigma);
	turnback[5] = 10 * cos(phi); 
    // turnback长度为6
 }       

void Light :: GetThePlane(float* turnback,float x,float y, float z,float phi,float sigma){
	GetTheLine(turnback,x,y,z,phi,sigma);
}
    
void  Light :: GetTheAngle(float* turnback,float* a , float* b ){   // 通过两个定点确定a观测b时的观测角

	float c[3] ;
	c[0] = (0)[b] - (0)[a] ;
	c[1] =  (1)[b] - (1)[a] ;
	c[2] = (2)[b] - (2)[a];
	/*
	for(int i = 0 ;i < 3 ;i++){
		printf("C: %f\n",c[i]);
	}
	*/
	
	float distance = pow( pow(c[0],2) +  pow(c[1],2) + pow(c[2],2) , 0.5 );
	
	/*
	printf("distance : %f\n",distance);
	*/
	turnback[0] = acos( c[2] / distance );
	turnback[1] = atan( c[1] / c[0]);
	turnback[0] = 180 - Rad_to_Angle(turnback[0]);
	turnback[1] =  Rad_to_Angle( turnback[1]);
	if( turnback[1] < 0  ){
		turnback[1] = 180 + turnback[1];
	}
	
	
}

int main(){
	Light lig;
	/*
	float turnback[6];
	
	lig.rectangular_to_spherical(turnback,45,45,10);
	
	for(int i=0 ; i<6 ;i++){
			printf("C:%f\n",turnback[i]);
	}
	*/
	//printf("Ground:OK: %u\n",lig.AutoSetGround(54.74, 45, 54.74, 45,
	//									 54.74, 98.13, 51.67, 71.57) );
	//printf("Wall:  OK: %u\n",lig.AutoSetWall(54.74, 98.13, 51.67, 71.57) );
	for(int i = 0 ; i < 180 ; i++){
		lig.Flush(15,i);	
		printf("Phi: %f\t",lig.Phi);
		printf("Sigma: %f\n",lig.Sigma);
		printf("\n");
			
	}
	
} 
