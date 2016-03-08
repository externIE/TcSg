#pragma once
//Tree Grow Controls, by Taohg, 201404

extern double GetSpeed(int x1, int y1, int x2, int y2, DWORD dwTimes);

namespace ShinyNova{

	//绘制流光类的Plane
	class CTgPlaneObject:public PlaneObject 
	{
	public:
		CTgPlaneObject();
		virtual ~CTgPlaneObject();
		
		//绘制流光
		virtual void DrawFlowPic(SNG_PIC& pic, int x=0,int y=0,int width=0,int height=0, double angle=0,double alpha=1.0);
	};

	#define LIGHTFLOW_MODE_NORMAL		0x00000001	//直接绘制
	#define LIGHTFLOW_MODE_NOTALPHA		0x00000002	//仅对底图是实心点(alpha=255)绘制

	enum{
		MSG_LIGHTFLOW_BEGIN=MSG_CUSTOMER_BEGIN+1,
		MSG_LIGHTFLOW_UPDATE,
		MSG_LIGHTFLOW_END
	};

	class CTgLightFlow : public CShinyNovaGui //流光效果类
	{
	public:
		CTgLightFlow();
		~CTgLightFlow();
		
				void CreateGUI(
							int		width, 
							int		height, 
							SNG_PIC picBk, 
							SNG_PIC picFlow, 
							DWORD   freq, 
							double  speed, //freq间的移动距离
							DWORD   dwMode=LIGHTFLOW_MODE_NOTALPHA);
				void SetBackPic(SNG_PIC& pic);
				void SetFLowPic(SNG_PIC& pic);
				void ShowPic();
				void StartFlow();
				void StopFlow();
				void UpdateFlow();
		virtual BOOL OnMsg(SNG_MSG& Msg);
		virtual PlaneObject*    SNGAPI     CreatePlane(int nWidth,int nHeight,UINT backcolor=COLOR_TRANSPARENT);
		
		//void Show();

	protected:
		BOOL			m_bStartFlow;
		DWORD			m_dwFreq;
		double			m_speed;
		DWORD			m_dwBeginTick;		//起始时间点
		DWORD			m_dwLastTick;       //上个时间点
		DWORD			m_dwMode;
		EVENTHANDLE		m_hUpdate;
		SNG_PIC         m_picBack;
		SNG_PIC         m_picFlow;
		CTgPlaneObject* m_plBackGround;
	};

	class CTgLightFlowEx : public CShinyNovaGui //流光效果类,可添加一张底图
	{
	public:
		CTgLightFlowEx();
		~CTgLightFlowEx();
			
				void CreateGUI(
						int		width, 
						int		height, 
						SNG_PIC picBK,
						SNG_PIC picFlowBK, 
						SNG_PIC picFlow, 
						DWORD   freq, 
						double  speed, //freq间的移动距离
						DWORD   dwMode=LIGHTFLOW_MODE_NOTALPHA);
				void ShowPic();
				void SetBackPic(SNG_PIC& picBack, SNG_PIC& picFlowBk);
				void StartFlow();
				void StopFlow();
		//void Show();
				
	protected:
		SNG_PIC         m_picBack;//有投影的底图
		PlaneObject*	m_plBack; //底图
		CTgLightFlow*	m_pFlow;  //流光类
	};
}
