
// SemDownView.cpp: CSemDownView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "SemDown.h"
#endif

#include "SemDownDoc.h"
#include "SemDownView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSemDownView

IMPLEMENT_DYNCREATE(CSemDownView, CView)

BEGIN_MESSAGE_MAP(CSemDownView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CSemDownView 생성/소멸

HANDLE hSema;

CSemDownView::CSemDownView() noexcept
{
	// TODO: 여기에 생성 코드를 추가합니다.
	hSema = CreateSemaphore(NULL, 3, 3, NULL);
}

CSemDownView::~CSemDownView()
{
	CloseHandle(hSema);
}

BOOL CSemDownView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CSemDownView 그리기

void CSemDownView::OnDraw(CDC* /*pDC*/)
{
	CSemDownDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CSemDownView 인쇄

BOOL CSemDownView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CSemDownView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CSemDownView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}


// CSemDownView 진단

#ifdef _DEBUG
void CSemDownView::AssertValid() const
{
	CView::AssertValid();
}

void CSemDownView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSemDownDoc* CSemDownView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSemDownDoc)));
	return (CSemDownDoc*)m_pDocument;
}
#endif //_DEBUG


// CSemDownView 메시지 처리기
int Y;
DWORD WINAPI ThreadDownload(LPVOID p)	// return 값은 DWORD
{
	CSemDownView* pView = (CSemDownView*)p;
	CDC* pDC = pView->GetDC();

	srand(GetTickCount());
	int s = rand() % 5 + 1;//다운로드 속도를 실제처럼 보이려고 난수사용함

	// 지역변수 없으면 오류날수도 있음 , 만약 지역변수 안 썼으면 critical section이나 mutex로 처리해야함.

	Y += 20;
	int y = Y;
	pDC->TextOut(10, y, _T("대기중"));
	// Flush=내보낸다 
	GdiFlush();

	

	////////////////////////////////////////
	DWORD dw = WaitForSingleObject(hSema, INFINITE);

	CString strTemp;
	for (int i = 0; i < 100; i++)
	{
		strTemp.Format(_T("다운로드중 : %d%% 완료"), i);
		for (int j = 0; j < i; j++)
			strTemp += _T("|");
		pDC->TextOut(10, y, strTemp);
		Sleep(s * 20);//시간지연
	}

	////////////////////////////////////////
	ReleaseSemaphore(hSema, 1, NULL);

	pDC->TextOut(10, y, _T("다운로드를 완료했습니다"));
	pView->ReleaseDC(pDC);
	return 0;
}

void CSemDownView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	DWORD ThreadID;
	CloseHandle(CreateThread(NULL, 0, ThreadDownload, this, 0, &ThreadID));

	CView::OnLButtonDown(nFlags, point);
}
