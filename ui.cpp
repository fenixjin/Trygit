#include <windows.h>
#include <windowsx.h>

#include "resource.h"

extern "C"
{
#include "data.h"
#include "util.h"
}

#define MAX_LOADSTRING 100

#define IDL_LIST 400

/////////////////////////////////////////////////////////////////////////////
//
// ȫ�ֱ���
//
TCHAR szClassName[] = "FRAMECLASS";  // ����������
TCHAR szTitle[] = "PMS";      // �������

HINSTANCE hInst;    // ȫ��ʵ��
HWND hWnd;          // �����ھ��,����Ϊȫ�ֱ����������ݽ���
HWND listHwnd = NULL;  // ��Ϣ�б���

static int screenWidth, screenHeight;   // ��¼���ڿ�Ⱥ͸߶�
static int cxChar, cyChar;      // ��¼��׼��Ԫ�Ŀ�Ⱥ͸߶�

struct University *ptUny = NULL;       // ������У�����ͨ��ָ��
struct College *ptClg = NULL;          // ����Ժϵ�����ͨ��ָ��
struct Paper *ptPaper = NULL;          // ����������Ϣ��ͨ��ָ��

/////////////////////////////////////////////////////////////////////////////
//
// ����
//
ATOM RegisterWinClass(HINSTANCE hInstance);                         // ע��������
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);               // ��ʼ��������
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);      // �����ڵĻص�
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);                 // "About" ��Ļص�

INT_PTR CALLBACK GetClg(HWND, UINT, WPARAM, LPARAM);                // �ڸöԻ�����,�����У���,������������ʾ��Ӧ��Ժϵ�б�
INT_PTR CALLBACK GetPaper(HWND, UINT, WPARAM, LPARAM);              // �ڸöԻ�����,�����У��ź�Ժϵ����,������������ʾ��Ӧ�������б�

INT_PTR CALLBACK NewUny(HWND, UINT, WPARAM, LPARAM);                // �ڸöԻ������½���У��Ϣ
INT_PTR CALLBACK NewClg(HWND, UINT, WPARAM, LPARAM);                // �ڸöԻ������½�Ժϵ��Ϣ
INT_PTR CALLBACK NewPaper(HWND, UINT, WPARAM, LPARAM);              // �ڸöԻ������½�������Ϣ

INT_PTR CALLBACK EditUny(HWND, UINT, WPARAM, LPARAM);                // �ڸöԻ������޸ĸ�У��Ϣ
INT_PTR CALLBACK EditClg(HWND, UINT, WPARAM, LPARAM);                // �ڸöԻ������޸�Ժϵ��Ϣ
INT_PTR CALLBACK EditPaper(HWND, UINT, WPARAM, LPARAM);              // �ڸöԻ������޸�������Ϣ

INT_PTR CALLBACK DeleteUny(HWND, UINT, WPARAM, LPARAM);             // �ڸöԻ�����ɾ����У��Ϣ
INT_PTR CALLBACK DeleteClg(HWND, UINT, WPARAM, LPARAM);             // �ڸöԻ�����ɾ��Ժϵ��Ϣ
INT_PTR CALLBACK DeletePaper(HWND, UINT, WPARAM, LPARAM);           // �ڸöԻ�����ɾ��������Ϣ

INT_PTR CALLBACK SearchUnyByCode(HWND, UINT, WPARAM, LPARAM);       // �ڸöԻ����������У��Ų��Ҹ�У��Ϣ
INT_PTR CALLBACK SearchUnyByName(HWND, UINT, WPARAM, LPARAM);       // �ڸöԻ����������У���ֲ��Ҹ�У��Ϣ
INT_PTR CALLBACK SearchClgByName(HWND, UINT, WPARAM, LPARAM);       // �ڸöԻ����������У��ź�Ժϵ���Ʋ���Ժϵ��Ϣ
INT_PTR CALLBACK SearchPaperByAuthor(HWND, UINT, WPARAM, LPARAM);   // �ڸöԻ����������������ֲ���������Ϣ
INT_PTR CALLBACK SearchPaperByTitle(HWND, UINT, WPARAM, LPARAM);    // �ڸöԻ������������ı������������Ϣ

INT_PTR CALLBACK ShowUnyPaperAmount(HWND, UINT, WPARAM, LPARAM);        // �ڸöԻ����������У�����ʾ��У��������
INT_PTR CALLBACK SearchPaperByIssueLevel(HWND, UINT, WPARAM, LPARAM);   // �ڸöԻ����������ڴκ����ĵȼ�����������Ϣ

void readData();                // ��ȡ����
void createListWindow ();       // �����Ӵ�����ʾ��Ϣ�б�
void resizeListWindow ();       // �����ڴ�С�仯��,���µ�����Ϣ�б��ڴ�С

void sendUnyListToCB(HWND);                         // ��ָ����combobox������ʾ��У�б�
void sendClgListToCB(HWND, struct University*);     // ��ָ����combobox������ʾָ����У�µ�Ժϵ�б�

void showUnyPaperAAmountTopFive();                  // ��ʾ������ǰ��ĸ�У�б�
void showUnyPaperPerTeaDsc ();                      // ����У�б���ʦ�˾����ķ��������н���������ʾ

void showSingleUny(struct University *, bool);      // ��ʾ������У��Ϣ
void showSpecificUnyList (struct University *);     // ��ʾ�ض��ĸ�У��
void showUnyList();                                 // ��ʾ��У�����б�
void showClgList(struct University *);              // ��ʾָ����У�ڵ���Ժϵ�����б�
void showSpecificClgList(struct College *, bool, bool);   // ��ʾ�ض���Ժϵ��
void showPaperList(struct College *);               // ��ʾ��ʾָ��Ժϵ�����ı����б�
void showSpecificPaperList(struct Paper *);         // ��ʾ�ض���������

/////////////////////////////////////////////////////////////////////////////
//
// �������
//
int WINAPI WinMain (HINSTANCE hThisInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR lpszArgument,
                    int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpszArgument);

    MSG msg;

    RegisterWinClass(hThisInstance);

    if (!InitInstance(hThisInstance, nCmdShow))
    {
        MessageBox (NULL,
                    TEXT ("Error Occured While Registerring Class!"),
                    TEXT ("Error Occured!"),
                    MB_ICONERROR);
        return false;
    }

    // ����Ϣѭ��
    while (GetMessage (&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

/////////////////////////////////////////////////////////////////////////////
//
// ����ʵ��
//
/**
 * ע��������
 */
ATOM RegisterWinClass(HINSTANCE hInstance)
{
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    wincl.cbSize = sizeof (WNDCLASSEX);

    wincl.hInstance = hInstance;
    wincl.lpszMenuName = MAKEINTRESOURCE(IDC_PROG);
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;
    wincl.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
    wincl.hIcon = LoadIcon (hInstance, MAKEINTRESOURCE(IDI_PROG));
    wincl.hIconSm = LoadIcon (wincl.hInstance, MAKEINTRESOURCE(IDI_PROG));
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;
    wincl.hbrBackground = (HBRUSH) COLOR_WINDOW;

    return RegisterClassEx(&wincl);
}

/**
 * ����ʵ�����������������
 */
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    //HWND hWnd;
    hInst = hInstance; // ��ʵ�����������ȫ�ֱ�����

    hWnd = CreateWindow (
               szClassName, /* Classname */
               szTitle, /* Title Text */
               WS_OVERLAPPEDWINDOW, /* default window */
               CW_USEDEFAULT,   /* Windows decides the position */
               0,   /* where the window ends up on the screen */
               960,   /* The programs width */
               540,   /* and height in pixels */
               NULL,    /* The window is a child-window to desktop */
               NULL,    /* No menu */
               hInstance,   /* Program Instance handler */
               NULL /* No Window Creation data */
           );

    if (!hWnd)
    {
        return false;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return true;
}

/**
 * �����ڻص�
 */
LRESULT CALLBACK WindowProcedure (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int wmEvent, wmId;
    TEXTMETRIC textMetric;
    HDC hdc;
    PAINTSTRUCT paintStruct;

    switch (message)                  /* handle the messages */
    {
    case WM_CREATE:
    {
        // ��ȡ��׼�ֿ���ָ�
        hdc = GetDC(hWnd);
        GetTextMetrics(hdc, &textMetric);
        cxChar = textMetric.tmAveCharWidth;
        cyChar = textMetric.tmHeight + textMetric.tmExternalLeading;
        ReleaseDC(hWnd, hdc);

        break;
    }

    case WM_SIZE:

        screenHeight = HIWORD(lParam);
        screenWidth = LOWORD(lParam);

        if (listHwnd == NULL)
        {
            createListWindow();
            readData();
        }

        resizeListWindow();
        break;

    case WM_COMMAND:

        wmEvent = HIWORD(wParam);
        wmId = LOWORD(wParam);

        switch (wmId)
        {


        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;

        case IDM_FILE_SAVE:
            writeOutList();
            break;

        case IDM_VIEW_U:
            showUnyList();
            break;

        case IDM_VIEW_C:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEW_GET_C), hWnd, GetClg);
            break;

        case IDM_VIEW_P:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_VIEW_GET_P), hWnd, GetPaper);
            break;

        case IDM_NEW_UNY:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_NEW_U), hWnd, NewUny);
            break;

        case IDM_NEW_CLG:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_NEW_C), hWnd, NewClg);
            break;

        case IDM_NEW_PAPER:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_NEW_P), hWnd, NewPaper);
            break;

        case IDM_DELETE_U:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DELETE_UNY), hWnd, DeleteUny);
            break;

        case IDM_DELETE_C:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DELETE_C), hWnd, DeleteClg);
            break;

        case IDM_DELETE_P:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_DELETE_P), hWnd, DeletePaper);
            break;

        case IDM_SEARCH_UNY_BY_CODE:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_SEARCH_U_BY_CODE), hWnd, SearchUnyByCode);
            break;

        case IDM_SEARCH_UNY_BY_NAME:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_SEARCH_U_BY_NAME), hWnd, SearchUnyByName);
            break;

        case IDM_SEARCH_CLG_BY_NAME:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_SEARCH_C_BY_NAME), hWnd, SearchClgByName);
            break;

        case IDM_SEARCH_CLG_BY_AMOUNT:
            showSpecificClgList(findAllClgTeaAmountOver200(), false, false);
            break;

        case IDM_SEARCH_PAPER_BY_AUTHOR:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_SEARCH_P_BY_AUTHOR), hWnd, SearchPaperByAuthor);
            break;

        case IDM_SEARCH_PAPER_BY_TITLE:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_SEARCH_P_BY_TITLE), hWnd, SearchPaperByTitle);
            break;

        case IDM_EDIT_UNY:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_U), hWnd, EditUny);
            break;

        case IDM_EDIT_CLG:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_C), hWnd, EditClg);
            break;

        case IDM_EDIT_PAPER:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_EDIT_P), hWnd, EditPaper);
            break;

        case IDM_STATISTIC_U_PAPER_SPECIFIC:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTIC_U_PAPER_SPECIFIC), hWnd, ShowUnyPaperAmount);
            break;

        case IDM_STATISTIC_P_ISSUE_SPECIFIC_LEVEL:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_STATISTIC_P_ISSUE_SPECIFIC_LEVEL), hWnd, SearchPaperByIssueLevel);
            break;

        case IDM_STATISITC_U_PAPER_A_TOP_FIVE:
            showUnyPaperAAmountTopFive();
            break;

        case IDM_STATISTIC_U_PAPER_PER_TEA_DSC:
            showUnyPaperPerTeaDsc();
            break;

        case IDM_STATISTIC_C_PAPER_TOP_TEN:
            showSpecificClgList(getClgDscByPaperAmount(), true, false);
            break;

        case IDM_FILE_EXIT:
            // �˳�����
            writeOutList();
            DestroyWindow(hWnd);
            break;
        default :
            return DefWindowProc(hWnd, message, wParam, lParam);
        }

        break;

    case WM_DESTROY:
        writeOutList();
        PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
        break;

    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hWnd, message, wParam, lParam);
    }

    return 0;
}

/**
 * "About"��Ļص�
 */
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�����У���,��������ʾ��Ӧ��Ժϵ�б�
 */
INT_PTR CALLBACK GetClg(HWND hGCDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND edHwnd;
    switch (msg)
    {
    case WM_INITDIALOG:
        edHwnd = GetDlgItem(hGCDlg, IDD_VIEW_GET_C_EDIT);
        Edit_LimitText(edHwnd, 7);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
        {
            char code[8];
            Edit_GetText(edHwnd, code, 8);
            struct University *ppUny = findUnyByCode(code);
            if (ppUny == NULL)
            {
                MessageBox(hGCDlg, "û�ҵ��ø�У", "��ʾ", NULL);
                break;
            }
            showClgList(ppUny);
        }
        case IDCANCEL:
            EndDialog(hGCDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�����У��ź�Ժϵ����,��������ʾ��Ӧ�������б�
 */
INT_PTR CALLBACK GetPaper(HWND hGPDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND EUHwnd, ECHwnd;
    switch (msg)
    {
    case WM_INITDIALOG:
        EUHwnd = GetDlgItem(hGPDlg, IDD_VIEW_GET_P_UNYCODE);
        ECHwnd = GetDlgItem(hGPDlg, IDD_VIEW_GET_P_CLGNAME);
        Edit_LimitText(EUHwnd, 7);
        Edit_LimitText(ECHwnd, 19);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
        {
            char unyCode[8], clgName[20];
            Edit_GetText(EUHwnd, unyCode, 8);
            Edit_GetText(ECHwnd, clgName, 20);
            struct University *ppUny = findUnyByCode(unyCode);
            if (ppUny == NULL)
            {
                MessageBox(hGPDlg, "û�ҵ��ø�У", "��ʾ", NULL);
                break;
            }
            struct College *ppClg = findClgByName(clgName, ppUny);
            if (ppClg == NULL)
            {
                MessageBox(hGPDlg, "û�ҵ���Ժϵ", "��ʾ", NULL);
                break;
            }
            showPaperList(ppClg);
        }
        case IDCANCEL:
            EndDialog(hGPDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�½���У��Ϣ
 */
INT_PTR CALLBACK NewUny(HWND hNewUDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND hCode, hName, hPrin, hPhone;
    switch (msg)
    {
    case WM_INITDIALOG:
        hCode = GetDlgItem(hNewUDlg, IDD_NEW_U_CODE);
        Edit_LimitText(hCode, 7);
        hName = GetDlgItem(hNewUDlg, IDD_NEW_U_NAME);
        Edit_LimitText(hName, 29);
        hPrin = GetDlgItem(hNewUDlg, IDD_NEW_U_PRIN);
        Edit_LimitText(hPrin, 14);
        hPhone = GetDlgItem(hNewUDlg, IDD_NEW_U_PHONE);
        Edit_LimitText(hPhone, 14);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
        {
            char code[8], name[30], prin[15], phone[15];
            Edit_GetText(hCode, code, 8);
            Edit_GetText(hName, name, 30);
            Edit_GetText(hPrin, prin, 15);
            Edit_GetText(hPhone, phone, 15);

            if (isStrContainLetter(code) || isStrContainLetter(phone))
            {
                MessageBox(hNewUDlg, "������Ϣ����", "��ʾ", NULL);
                return (INT_PTR)TRUE;
            }

            newUnyNode(code, name, prin, phone);
            MessageBox(hNewUDlg, "�½���У��Ϣ�ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        }
        case IDCANCEL:
            EndDialog(hNewUDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�½�Ժϵ��Ϣ
 */
INT_PTR CALLBACK NewClg(HWND hNewCDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND cbHwnd, edName, edTeaNum, edProNum, edAProNum, edBclrNum;
    static University *pCurUny;

    switch (msg)
    {
    case WM_INITDIALOG:
        cbHwnd = GetDlgItem(hNewCDlg, IDD_NEW_C_UCODE);
        edName = GetDlgItem(hNewCDlg, IDD_NEW_C_NAME);
        Edit_LimitText(edName, 19);
        edTeaNum = GetDlgItem(hNewCDlg, IDD_NEW_C_T_NUM);
        Edit_LimitText(edTeaNum, 100);
        edProNum = GetDlgItem(hNewCDlg, IDD_NEW_C_P_NUM);
        Edit_LimitText(edProNum, 100);
        edAProNum = GetDlgItem(hNewCDlg, IDD_NEW_C_AP_NUM);
        Edit_LimitText(edAProNum, 100);
        edBclrNum = GetDlgItem(hNewCDlg, IDD_NEW_C_B_NUM);
        Edit_LimitText(edBclrNum, 100);

        sendUnyListToCB(cbHwnd);

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDD_NEW_C_UCODE:
            switch(wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(cbHwnd);
                if (iCurSel < 0)
                {
                    MessageBox(hNewCDlg, "error", "hint", NULL);
                    return (INT_PTR)TRUE;
                }
                char str[100];
                ComboBox_GetLBText(cbHwnd, iCurSel, str);
                pCurUny = findUnyByCode(peekUnyCode(str));
                break;
            }
            return (INT_PTR)TRUE;
        case IDOK:
        {
            char teaNum[100], proNum[100], aProNum[100], bclrNum[100];
            Edit_GetText(edTeaNum, teaNum, 100);
            Edit_GetText(edProNum, proNum, 100);
            Edit_GetText(edAProNum, aProNum, 100);
            Edit_GetText(edBclrNum, bclrNum, 100);
            if (isStrContainLetter(teaNum)
                    || isStrContainLetter(proNum)
                    || isStrContainLetter(aProNum)
                    || isStrContainLetter(bclrNum))
            {
                MessageBox(hNewCDlg, "��������", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            char name[20];
            Edit_GetText(edName, name, 20);
            int teaNumber = atoi(teaNum);
            int proNumber = atoi(proNum);
            int aProNumber = atoi(aProNum);
            int bclrNumber = atoi(bclrNum);

            newCollegeNode(pCurUny,
                           name,
                           teaNumber,
                           proNumber,
                           aProNumber,
                           bclrNumber);
            MessageBox(hNewCDlg, "�½�Ժϵ��Ϣ�ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        }
        case IDCANCEL:
            EndDialog(hNewCDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�½�������Ϣ
 */
INT_PTR CALLBACK NewPaper(HWND hNewPDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND cbUHwnd, cbCHwnd, edTitle, edAuthor, edJIssue, edJName, edLevel;
    static struct University *pCurUny;
    static struct College *pCurClg;
    switch (msg)
    {
    case WM_INITDIALOG:
        cbUHwnd = GetDlgItem(hNewPDlg, IDD_NEW_P_UCODE);
        cbCHwnd = GetDlgItem(hNewPDlg, IDD_NEW_P_CNAME);
        edTitle = GetDlgItem(hNewPDlg, IDD_NEW_P_TITLE);
        Edit_LimitText(edTitle, 50);
        edAuthor = GetDlgItem(hNewPDlg, IDD_NEW_P_AUTHOR);
        Edit_LimitText(edAuthor, 15);
        edJIssue = GetDlgItem(hNewPDlg, IDD_NEW_P_JISSUE);
        Edit_LimitText(edJIssue, 10);
        edJName = GetDlgItem(hNewPDlg, IDD_NEW_P_JNAME);
        Edit_LimitText(edJName, 20);
        edLevel = GetDlgItem(hNewPDlg, IDD_NEW_P_LEVEL);
        Edit_LimitText(edLevel, 1);

        sendUnyListToCB(cbUHwnd);

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDD_NEW_P_UCODE:
            switch (wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(cbUHwnd);
                if (iCurSel < 0)
                    break;
                char str[100];
                ComboBox_GetLBText(cbUHwnd, iCurSel, str);
                pCurUny = findUnyByCode(peekUnyCode(str));
                if (pCurUny == NULL)
                    MessageBox(hNewPDlg, "uny pointer null", "hint", NULL);

                sendClgListToCB(cbCHwnd, pCurUny);
                break;
            }
            break;

        case IDD_NEW_P_CNAME:
            switch(wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(cbCHwnd);
                if (iCurSel < 0)
                    break;
                char str[100];
                ComboBox_GetLBText(cbCHwnd, iCurSel, str);
                pCurClg = findClgByName(str, pCurUny);
                if (pCurClg == NULL)
                {
                    MessageBox(hNewPDlg, "clg pointer null", "hint", NULL);
                    return (INT_PTR)TRUE;
                }
                break;
            }
            return (INT_PTR)TRUE;

        case IDOK:
        {
            char jIssue[10];
            Edit_GetText(edJIssue, jIssue, 10);
            if (isStrContainLetter(jIssue))
            {
                MessageBox(hNewPDlg, "�ڴ���������", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            char level[2];
            Edit_GetText(edLevel, level, 2);
            if (!(level[0] > 64 && level[0] < 69 || level[0] > 96 && level[0] < 101))
            {
                MessageBox(hNewPDlg, "���ĵȼ���������", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            char title[50];
            Edit_GetText(edTitle, title, 50);
            char author[15], jName[20];
            Edit_GetText(edAuthor, author, 15);
            Edit_GetText(edJName, jName, 20);

            newPaperNode(pCurClg,
                         author,
                         title,
                         level[0],
                         jName,
                         jIssue);
            MessageBox(hNewPDlg, "�½�������Ϣ�ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        }
        case IDCANCEL:
            EndDialog(hNewPDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�޸ĸ�У��Ϣ
 */
INT_PTR CALLBACK EditUny(HWND hEditUDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND hCode, hName, hPrin, hPhone;
    switch (msg)
    {
    case WM_INITDIALOG:
        hCode = GetDlgItem(hEditUDlg, IDD_EDIT_U_CODE);
        Edit_LimitText(hCode, 7);
        hName = GetDlgItem(hEditUDlg, IDD_EDIT_U_NAME);
        Edit_LimitText(hName, 29);
        hPrin = GetDlgItem(hEditUDlg, IDD_EDIT_U_PRIN);
        Edit_LimitText(hPrin, 14);
        hPhone = GetDlgItem(hEditUDlg, IDD_EDIT_U_PHONE);
        Edit_LimitText(hPhone, 14);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
        {
            char code[8];
            Edit_GetText(hCode, code, 8);
            if (isStrContainLetter(code) || (ptUny = findUnyByCode(code)) == NULL)
            {
                MessageBox(hEditUDlg, "��У�����������,û�ҵ���Ӧ��У", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }
            char name[30], prin[15], phone[15];
            Edit_GetText(hName, name, 30);
            Edit_GetText(hPrin, prin, 15);
            Edit_GetText(hPhone, phone, 15);

            if (isStrContainLetter(phone))
            {
                MessageBox(hEditUDlg, "������Ϣ����", "��ʾ", NULL);
                return (INT_PTR)TRUE;
            }

            strcpy(ptUny->unyName, name);
            strcpy(ptUny->unyPrincipal, prin);
            strcpy(ptUny->unyPhone, phone);
            MessageBox(hEditUDlg, "�޸ĸ�У��Ϣ�ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        }
        case IDCANCEL:
            EndDialog(hEditUDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�޸�Ժϵ��Ϣ
 */
INT_PTR CALLBACK EditClg(HWND hEditCDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND cbHwnd, edName, edTeaNum, edProNum, edAProNum, edBclrNum;
    static University *pCurUny;

    switch (msg)
    {
    case WM_INITDIALOG:
        cbHwnd = GetDlgItem(hEditCDlg, IDD_EDIT_C_UCODE);
        edName = GetDlgItem(hEditCDlg, IDD_EDIT_C_NAME);
        Edit_LimitText(edName, 19);
        edTeaNum = GetDlgItem(hEditCDlg, IDD_EDIT_C_T_NUM);
        Edit_LimitText(edTeaNum, 100);
        edProNum = GetDlgItem(hEditCDlg, IDD_EDIT_C_P_NUM);
        Edit_LimitText(edProNum, 100);
        edAProNum = GetDlgItem(hEditCDlg, IDD_EDIT_C_AP_NUM);
        Edit_LimitText(edAProNum, 100);
        edBclrNum = GetDlgItem(hEditCDlg, IDD_EDIT_C_B_NUM);
        Edit_LimitText(edBclrNum, 100);

        sendUnyListToCB(cbHwnd);

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDD_EDIT_C_UCODE:
            switch(wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(cbHwnd);
                if (iCurSel < 0)
                {
                    MessageBox(hEditCDlg, "error", "hint", NULL);
                    return (INT_PTR)TRUE;
                }
                char str[100];
                ComboBox_GetLBText(cbHwnd, iCurSel, str);
                pCurUny = findUnyByCode(peekUnyCode(str));
                break;
            }
            return (INT_PTR)TRUE;
        case IDOK:
        {
            char teaNum[100], proNum[100], aProNum[100], bclrNum[100];
            Edit_GetText(edTeaNum, teaNum, 100);
            Edit_GetText(edProNum, proNum, 100);
            Edit_GetText(edAProNum, aProNum, 100);
            Edit_GetText(edBclrNum, bclrNum, 100);
            if (isStrContainLetter(teaNum)
                    || isStrContainLetter(proNum)
                    || isStrContainLetter(aProNum)
                    || isStrContainLetter(bclrNum))
            {
                MessageBox(hEditCDlg, "��������", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            char name[20];
            Edit_GetText(edName, name, 20);
            struct College *ppClg;
            if ((ppClg = findClgByName(name, pCurUny)) == NULL)
            {
                MessageBox(hEditCDlg, "Ժϵ������������,�Ҳ�����Ժϵ", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }
            ppClg->belongUny->amountTeacher -= ppClg->amountTeacher;
            int teaNumber = atoi(teaNum);
            int proNumber = atoi(proNum);
            int aProNumber = atoi(aProNum);
            int bclrNumber = atoi(bclrNum);

            ppClg->amountTeacher = teaNumber;
            ppClg->amountProfessor = proNumber;
            ppClg->amountAssocProfessor = aProNumber;
            ppClg->amountBachelor = bclrNumber;
            ppClg->belongUny->amountTeacher += ppClg->amountTeacher;
            MessageBox(hEditCDlg, "Ժϵ��Ϣ�޸ĳɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        }
        case IDCANCEL:
            EndDialog(hEditCDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�޸�������Ϣ
 */
INT_PTR CALLBACK EditPaper(HWND hEditPDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND cbUHwnd, cbCHwnd, edTitle, edAuthor, edJIssue, edJName, edLevel;
    static struct University *pCurUny;
    static struct College *pCurClg;
    switch (msg)
    {
    case WM_INITDIALOG:
        cbUHwnd = GetDlgItem(hEditPDlg, IDD_EDIT_P_UCODE);
        cbCHwnd = GetDlgItem(hEditPDlg, IDD_EDIT_P_CNAME);
        edTitle = GetDlgItem(hEditPDlg, IDD_EDIT_P_TITLE);
        Edit_LimitText(edTitle, 50);
        edAuthor = GetDlgItem(hEditPDlg, IDD_EDIT_P_AUTHOR);
        Edit_LimitText(edAuthor, 15);
        edJIssue = GetDlgItem(hEditPDlg, IDD_EDIT_P_JISSUE);
        Edit_LimitText(edJIssue, 10);
        edJName = GetDlgItem(hEditPDlg, IDD_EDIT_P_JNAME);
        Edit_LimitText(edJName, 20);
        edLevel = GetDlgItem(hEditPDlg, IDD_EDIT_P_LEVEL);
        Edit_LimitText(edLevel, 1);

        sendUnyListToCB(cbUHwnd);

        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDD_EDIT_P_UCODE:
            switch (wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(cbUHwnd);
                if (iCurSel < 0)
                    break;
                char str[100];
                ComboBox_GetLBText(cbUHwnd, iCurSel, str);
                pCurUny = findUnyByCode(peekUnyCode(str));
                if (pCurUny == NULL)
                    MessageBox(hEditPDlg, "uny pointer null", "hint", NULL);

                sendClgListToCB(cbCHwnd, pCurUny);
                break;
            }
            break;

        case IDD_EDIT_P_CNAME:
            switch(wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(cbCHwnd);
                if (iCurSel < 0)
                    break;
                char str[100];
                ComboBox_GetLBText(cbCHwnd, iCurSel, str);
                pCurClg = findClgByName(str, pCurUny);
                if (pCurClg == NULL)
                {
                    MessageBox(hEditPDlg, "clg pointer null", "hint", NULL);
                    return (INT_PTR)TRUE;
                }
                break;
            }
            return (INT_PTR)TRUE;

        case IDOK:
        {
            char jIssue[10];
            Edit_GetText(edJIssue, jIssue, 10);
            if (isStrContainLetter(jIssue))
            {
                MessageBox(hEditPDlg, "�ڴ���������", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            char level[2];
            Edit_GetText(edLevel, level, 2);
            if (!(level[0] > 64 && level[0] < 69 || level[0] > 96 && level[0] < 101))
            {
                MessageBox(hEditPDlg, "���ĵȼ���������", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            char title[50];
            Edit_GetText(edTitle, title, 50);
            struct Paper *ppPaper;
            if ((ppPaper = findPaperByTitle(title, pCurClg)) == NULL)
            {
                MessageBox(hEditPDlg, "���ı�����������,û�ҵ���Ӧ������Ϣ", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }

            if (ppPaper->paperLevel == 'a' || ppPaper->paperLevel == 'A')
            {
                ppPaper->belongClg->belongUny->amountPaperA--;
            }
            else if (ppPaper->paperLevel == 'b' || ppPaper->paperLevel == 'B')
            {
                ppPaper->belongClg->belongUny->amountPaperB--;
            }
            else if (ppPaper->paperLevel == 'c' || ppPaper->paperLevel == 'C')
            {
                ppPaper->belongClg->belongUny->amountPaperC--;
            }
            else if (ppPaper->paperLevel == 'd' || ppPaper->paperLevel == 'D')
            {
                ppPaper->belongClg->belongUny->amountPaperD--;
            }

            char author[15], jName[20];
            Edit_GetText(edAuthor, author, 15);
            Edit_GetText(edJName, jName, 20);

            strcpy(ppPaper->paperAuthor, author);
            strcpy(ppPaper->journalIssue, jIssue);
            strcpy(ppPaper->journalName, jName);
            ppPaper->paperLevel = level[0];

            if (level[0] == 'a' || level[0] == 'A')
            {
                ppPaper->belongClg->belongUny->amountPaperA++;
            }
            else if (level[0] == 'b' || level[0] == 'B')
            {
                ppPaper->belongClg->belongUny->amountPaperB++;
            }
            else if (level[0] == 'c' || level[0] == 'C')
            {
                ppPaper->belongClg->belongUny->amountPaperC++;
            }
            else if (level[0] == 'd' || level[0] == 'D')
            {
                ppPaper->belongClg->belongUny->amountPaperD++;
            }
            MessageBox(hEditPDlg, "�޸�������Ϣ�ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        }
        case IDCANCEL:
            EndDialog(hEditPDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,ɾ����У��Ϣ
 */
INT_PTR CALLBACK DeleteUny(HWND hDUDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static HWND cHwnd;
    switch (msg)
    {
    case WM_INITDIALOG:
        cHwnd = GetDlgItem(hDUDlg, IDD_DELETE_U_CODE);
        Edit_LimitText(cHwnd, 7);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
            char code[8];
            Edit_GetText(cHwnd, code, 8);
            deleteUnyByCode(code);
            MessageBox(hDUDlg, "ɾ���ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        case IDCANCEL:
            EndDialog(hDUDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR) FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,ɾ��Ժϵ��Ϣ
 */
INT_PTR CALLBACK DeleteClg(HWND hDCDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static struct University *pCurUny;
    static HWND hUCodeCB, hCNameEd;
    switch (msg)
    {
    case WM_INITDIALOG:
        hUCodeCB = GetDlgItem(hDCDlg, IDD_DELETE_C_UCODE);
        hCNameEd = GetDlgItem(hDCDlg, IDD_DELETE_C_NAME);
        Edit_LimitText(hCNameEd, 20);
        sendUnyListToCB(hUCodeCB);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDD_DELETE_C_UCODE:
            switch (wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(hUCodeCB);
                char str[100];
                ComboBox_GetLBText(hUCodeCB, iCurSel, str);
                pCurUny = findUnyByCode(peekUnyCode(str));
                break;
            }
            return (INT_PTR)TRUE;
        case IDOK:
            char sCName[20];
            Edit_GetText(hCNameEd, sCName, 20);
            deleteClgByName(sCName, pCurUny);
            MessageBox(hDCDlg, "Ժϵɾ���ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        case IDCANCEL:
            EndDialog(hDCDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի�����
 * �ڸöԻ�����,ɾ��������Ϣ
 */
INT_PTR CALLBACK DeletePaper(HWND hDPDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static struct University *pCurUny;
    static struct College *pCurClg;
    static HWND hUCodeCB, hCNameCB, hPTitleEd;
    switch (msg)
    {
    case WM_INITDIALOG:
        hUCodeCB = GetDlgItem(hDPDlg, IDD_DELETE_P_UCODE);
        hCNameCB = GetDlgItem(hDPDlg, IDD_DELETE_P_CNAME);
        hPTitleEd = GetDlgItem(hDPDlg, IDD_DELETE_P_TITLE);
        Edit_LimitText(hPTitleEd, 50);

        sendUnyListToCB(hUCodeCB);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDD_DELETE_P_UCODE:
            switch (wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(hUCodeCB);
                if (iCurSel < 0)
                    break;
                char str[100];
                ComboBox_GetLBText(hUCodeCB, iCurSel, str);
                pCurUny = findUnyByCode(peekUnyCode(str));
                if (pCurUny == NULL)
                    MessageBox(hDPDlg, "uny pointer null", "hint", NULL);

                sendClgListToCB(hCNameCB, pCurUny);
                break;
            }
            return (INT_PTR)TRUE;

        case IDD_DELETE_P_CNAME:
            switch(wmEvent)
            {
            case CBN_SELCHANGE:
                int iCurSel = ComboBox_GetCurSel(hCNameCB);
                if (iCurSel < 0)
                    break;
                char str[100];
                ComboBox_GetLBText(hCNameCB, iCurSel, str);
                pCurClg = findClgByName(str, pCurUny);
                if (pCurClg == NULL)
                {
                    MessageBox(hDPDlg, "clg pointer null", "hint", NULL);
                    return (INT_PTR)TRUE;
                }
                break;
            }
            return (INT_PTR)TRUE;

        case IDOK:
            char title[50];
            Edit_GetText(hPTitleEd, title, 50);
            deletePaperByTitle(title, pCurUny, pCurClg);
            MessageBox(hDPDlg, "����ɾ���ɹ�", "��ʾ", NULL);
            SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
        case IDCANCEL:
            EndDialog(hDPDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի�����
 * �ڸöԻ�����,�����У��Ų��Ҹ�У��Ϣ
 */
INT_PTR CALLBACK SearchUnyByCode(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static HWND hEdit;
    switch (msg)
    {
    case WM_INITDIALOG:
        hEdit = GetDlgItem(hDlg, IDD_SEARCH_U_BY_CODE_E);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
        {
            char str[100];
            Edit_GetText(hEdit, str, 100);
            if (isStrContainLetter(str))
            {
                MessageBox(hDlg, "����������", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }
            showSingleUny(findUnyByCode(str), false);
        }

        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի�����
 * �ڸöԻ�����,�����У���ֲ��Ҹ�У��Ϣ
 */
INT_PTR CALLBACK SearchUnyByName(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static HWND hEdit;
    switch (msg)
    {
    case WM_INITDIALOG:
        hEdit = GetDlgItem(hDlg, IDD_SEARCH_U_BY_NAME_E);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
            char str[100];
            Edit_GetText(hEdit, str, 100);
            showSpecificUnyList(findUnyByString(str));
        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�����У��ź�Ժϵ����,��������ʾ��Ӧ��Ժϵ��Ϣ
 */
INT_PTR CALLBACK SearchClgByName(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND hUCodeEd, hCNameEd;
    switch (msg)
    {
    case WM_INITDIALOG:
        hUCodeEd = GetDlgItem(hDlg, IDD_SEARCH_C_BY_NAME_UCODE);
        hCNameEd = GetDlgItem(hDlg, IDD_SEARCH_C_BY_NAME_CNAME);
        Edit_LimitText(hUCodeEd, 7);
        Edit_LimitText(hCNameEd, 19);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
        {
            char unyCode[8], clgName[20];
            Edit_GetText(hUCodeEd, unyCode, 8);
            Edit_GetText(hCNameEd, clgName, 20);
            struct University *ppUny = findUnyByCode(unyCode);
            if (ppUny == NULL)
            {
                MessageBox(hDlg, "û�ҵ��ø�У", "��ʾ", NULL);
                break;
            }
            struct College *ppClg = findClgByName(clgName, ppUny);
            if (ppClg == NULL)
            {
                MessageBox(hDlg, "û�ҵ���Ժϵ", "��ʾ", NULL);
                break;
            }
            showSpecificClgList(ppClg, false, true);
        }
        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի�����
 * �ڸöԻ�����,�����������ֲ���������Ϣ
 */
INT_PTR CALLBACK SearchPaperByAuthor(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static HWND hEdit;
    switch (msg)
    {
    case WM_INITDIALOG:
        hEdit = GetDlgItem(hDlg, IDD_SEARCH_P_BY_AUTHOR_E);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
            char str[100];
            Edit_GetText(hEdit, str, 100);
            showSpecificPaperList(findAllPaperByAuthor(str));

        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի�����
 * �ڸöԻ�����,�������ı������������Ϣ
 */
INT_PTR CALLBACK SearchPaperByTitle(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static HWND hEdit;
    switch (msg)
    {
    case WM_INITDIALOG:
        hEdit = GetDlgItem(hDlg, IDD_SEARCH_P_BY_TITLE_E);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
            char str[100];
            Edit_GetText(hEdit, str, 100);
            showSpecificPaperList(findAllPaperByTitle(str));
        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի���ص�
 * �ڸöԻ�����,�����У�����ʾ��������
 */
INT_PTR CALLBACK ShowUnyPaperAmount(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    static HWND edHwnd;
    switch (msg)
    {
    case WM_INITDIALOG:
        edHwnd = GetDlgItem(hDlg, IDD_STATISTIC_U_PAPER_SPECIFIC_ED);
        Edit_LimitText(edHwnd, 7);
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
        {
            char code[8];
            Edit_GetText(edHwnd, code, 8);
            struct University *ppUny = findUnyByCode(code);
            if (ppUny == NULL)
            {
                MessageBox(hDlg, "û�ҵ��ø�У", "��ʾ", NULL);
                return (INT_PTR)TRUE;
            }

            showSingleUny(ppUny, true);
        }
        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի�����
 * �ڸöԻ�����,�����ڴκ͵ȼ�����������Ϣ
 */
INT_PTR CALLBACK SearchPaperByIssueLevel(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static HWND hLevelEd, hIssueEd;
    switch (msg)
    {
    case WM_INITDIALOG:
        hLevelEd = GetDlgItem(hDlg, IDD_STATISTIC_P_ISSUE_LEVEL_LEVEL);
        Edit_LimitText(hLevelEd, 1);
        hIssueEd = GetDlgItem(hDlg, IDD_STATISTIC_P_ISSUE_LEVEL_ISSUE);
        Edit_LimitText(hIssueEd, 9);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
            char issue[10], level[2];
            Edit_GetText(hIssueEd, issue, 10);
            Edit_GetText(hLevelEd, level, 2);
            showSpecificPaperList(getPaperListByIssueLevel(issue, level[0]));

        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * �Ի�����
 * �ڸöԻ�����,�����У��ź�����ҳ��о����˾�����ǰʮ��Ժϵ
 */
INT_PTR CALLBACK ShowPaperPerBclrYearInUny(HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(wParam);
    static HWND hUCodeEd, hYearEd;
    switch (msg)
    {
    case WM_INITDIALOG:
        hUCodeEd = GetDlgItem(hDlg, IDD_STATISTIC_U_PAPER_PER_BCLR_IN_YEAR_CODE);
        Edit_LimitText(hUCodeEd, 7);
        hYearEd = GetDlgItem(hDlg, IDD_STATISTIC_U_PAPER_PER_BCLR_IN_YEAR_YEAR);
        Edit_LimitText(hYearEd, 4);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);

        switch (wmId)
        {
        case IDOK:
            char code[8], year[5];
            Edit_GetText(hUCodeEd, code, 8);
            Edit_GetText(hYearEd, year, 5);
            ptUny = findUnyByCode(code);
            if (ptUny == NULL)
            {
                MessageBox(hDlg, "��У�����������, �Ҳ����ø�У", "��ʾ", MB_ICONERROR);
                return (INT_PTR)TRUE;
            }
            showSpecificClgList(getPaperPerBachelorMostTenYearInUny(year, ptUny), true, false);

        case IDCANCEL:
            EndDialog(hDlg, wmId);
            return (INT_PTR)TRUE;
        }
    }
    return (INT_PTR)FALSE;
}

/**
 * ��ȡ����
 */
void readData()
{
    if (readInList())
    {
        MessageBox(hWnd, "���ݶ�ȡ�ɹ�", "��ʾ", NULL);
        return;
    }

    ptUny = getListHead();
    ptUny = NULL;
    MessageBox(hWnd, "�����ļ�������", "��ʾ", NULL);
}

/**
 * ������ʾ��Ϣ�б���Ӵ���
 */
void createListWindow ()
{
    listHwnd = CreateWindowEx (
                   NULL,
                   "LISTBOX",
                   "listWindow",
                   WS_CHILDWINDOW | WS_VISIBLE | WS_BORDER | WS_VSCROLL,
                   cxChar,
                   cyChar,
                   screenWidth - cxChar*2,
                   screenHeight - cyChar,
                   hWnd,
                   (HMENU) IDL_LIST,
                   GetModuleHandle(NULL),
                   NULL);
}

/**
 * �ڸ����ڴ�С�仯��,���µ�����Ϣ�б��ڴ�С
 */
void resizeListWindow()
{
    MoveWindow(listHwnd,
               cxChar,
               cyChar,
               screenWidth - cxChar*2,
               screenHeight - cyChar,
               TRUE);
}

/**
 * Ŀ�� ��ʾ�ض��ĸ�У��
 *
 * ����
 *      pSrcList �����ĸ�У��
 */
void showSpecificUnyList (struct University *pSrcList)
{
    ptUny = pSrcList;
    if (ptUny == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }

    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);

    char str[100];

    while (ptUny)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptUny->unyName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���:  %s", ptUny->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ������  :  %s", ptUny->unyPrincipal);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��ϵ�绰:  %s", ptUny->unyPhone);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");
        ptUny = ptUny->nextUniversity;
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * ��ʾ������ǰ��ĸ�У�б�
 */
void showUnyPaperAAmountTopFive ()
{
    sortUnyByPaperAAmount();

    ptUny = getListHead();
    if (ptUny == NULL)
    {
        MessageBox(hWnd, "��У��������", "��ʾ", MB_ICONERROR);
        return;
    }

    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);

    char str[100];
    int i = 0;
    while (ptUny && i < 5)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptUny->unyName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���:  %s", ptUny->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��������:  %d", ptUny->amountPaper);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����A ��:  %d", ptUny->amountPaperA);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����B ��:  %d", ptUny->amountPaperB);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����C ��:  %d", ptUny->amountPaperC);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����D ��:  %d", ptUny->amountPaperD);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");

        ptUny = ptUny->nextUniversity;
        i++;
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * ����У�б���ʦ�˾����ķ��������н���������ʾ
 */
void showUnyPaperPerTeaDsc ()
{
    sortUnyByPaperPerTea();

    ptUny = getListHead();
    if (ptUny == NULL)
    {
        MessageBox(hWnd, "��У��������", "��ʾ", MB_ICONERROR);
        return;
    }

    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);

    char str[100];
    int i = 0;
    while (ptUny)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptUny->unyName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���      :  %s", ptUny->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��������      :  %d", ptUny->amountPaper);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��ʦ����      :  %d", ptUny->amountTeacher);
        ListBox_AddString(listHwnd, str);
        sprintf(str, "      ��ʦ�˾�������:  %g", ptUny->amountPaperPerTeacher);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");

        ptUny = ptUny->nextUniversity;
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * Ŀ�� ��ʾ������У��Ϣ
 *
 * ����
 *      ppUny ָ���ĸ�У���
 *      showPaperAmount ��Ϊ��,�������ʾ�ø�У�ڵ�Ļ�����Ϣ��,���������Ӧ��������
 */
void showSingleUny (struct University *ppUny, bool showPaperAmount)
{
    if (ppUny == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }

    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);

    char str[100];
    ListBox_AddString(listHwnd, " ");
    wsprintf(str, "    %s", ppUny->unyName);
    ListBox_AddString(listHwnd, str);
    wsprintf(str, "      ��У���:  %s", ppUny->unyCode);
    ListBox_AddString(listHwnd, str);
    wsprintf(str, "      ������  :  %s", ppUny->unyPrincipal);
    ListBox_AddString(listHwnd, str);
    wsprintf(str, "      ��ϵ�绰:  %s", ppUny->unyPhone);
    ListBox_AddString(listHwnd, str);
    ListBox_AddString(listHwnd, " ");

    if (showPaperAmount)
    {
        wsprintf(str, "      ��������:  %d", ppUny->amountPaper);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����A ��:  %d", ppUny->amountPaperA);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����B ��:  %d", ppUny->amountPaperB);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����C ��:  %d", ppUny->amountPaperC);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����D ��:  %d", ppUny->amountPaperD);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * Ŀ��:��ʾ��У�����б�
 */
void showUnyList ()
{
    showSpecificUnyList(getListHead());
}

/**
 * Ŀ��:��ʾָ����У�ڵ���Ժϵ�����б�
 *
 * ����:
 *      ptrUny ָ����У�ڵ�
 */
void showClgList(struct University *ptrUny)
{
    if (ptrUny == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }

    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);   // ���ԭ������
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);  // ��ֹ���濨ס�����

    ptClg = ptrUny->headCollege;
    if (ptClg == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }

    char str[100];
    while (ptClg)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptClg->clgName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���:    %s", ptClg->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��ʦ����:    %d", ptClg->amountTeacher);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��������:    %d", ptClg->amountProfessor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����������:  %d", ptClg->amountAssocProfessor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      �о�������:  %d", ptClg->amountBachelor);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");
        ptClg = ptClg->nextCollege;
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * Ŀ��:��ʾָ��Ժϵ�ڵ������������б�
 *
 * ����:
 *      ptrClg ָ��Ժϵ�ڵ�
 */
void showPaperList(struct College *ptrClg)
{
    if (ptrClg == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }

    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);

    ptPaper = ptrClg->headPaper;
    if (ptPaper == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }

    char str[100];
    while (ptPaper)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptPaper->paperTitle);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��һ����:    %s", ptPaper->paperAuthor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���:    %s", ptPaper->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����Ժϵ:    %s", ptPaper->clgName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����    :    %s", ptPaper->journalName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����    :    %s", ptPaper->journalIssue);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ���ķ���:    %c", ptPaper->paperLevel);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");
        ptPaper = ptPaper->nextPaper;
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * Ŀ��:��ʾ�ض���Ժϵ��
 *
 * ����
 *      pScrList ָ����Ժϵ������ͷ
 *      showPaperAmount ��Ϊ��,��ʵ������ͳ�Ƶ���ʾ������ǰʮ��Ժϵ
 *      showSingleInfo ��Ϊ��,��ʵ����ʾ����Ժϵ������Ϣ�Ĺ���
 *
 * ע��
 *      showPaperAmount �� showSingleInfo����ͬʱΪ��
 *      ��showPaperAmount �� showSingleInfo������,���������Ժϵ����Ժϵ�Ļ�����Ϣ
 */
void showSpecificClgList(struct College* pSrcList, bool showPaperAmount, bool showSingleInfo)
{
    ptClg = pSrcList;
    if (ptClg == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }
    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);   // ���ԭ������
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);  // ��ֹ���濨ס�����

    char str[100];
    if (showPaperAmount)
    {
        int count = 0;
        while (ptClg && count < 10)
        {
            ListBox_AddString(listHwnd, " ");
            wsprintf(str, "    %s", ptClg->clgName);
            ListBox_AddString(listHwnd, str);
            wsprintf(str, "      ��У���:    %s", ptClg->unyCode);
            ListBox_AddString(listHwnd, str);
            wsprintf(str, "      ��ʦ����:    %d", ptClg->amountTeacher);
            ListBox_AddString(listHwnd, str);
            wsprintf(str, "      ��������:    %d", ptClg->amountProfessor);
            ListBox_AddString(listHwnd, str);
            wsprintf(str, "      ����������:  %d", ptClg->amountAssocProfessor);
            ListBox_AddString(listHwnd, str);
            wsprintf(str, "      �о�������:  %d", ptClg->amountBachelor);
            ListBox_AddString(listHwnd, str);
            if (showPaperAmount)
            {
                wsprintf(str, "      ��������:    %d", ptClg->amountPaper);
            }
            ListBox_AddString(listHwnd, str);
            ListBox_AddString(listHwnd, " ");
            ptClg = ptClg->nextCollege;
            count++;
        }
        SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
        return;
    }

    if (showSingleInfo)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptClg->clgName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���:    %s", ptClg->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��ʦ����:    %d", ptClg->amountTeacher);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��������:    %d", ptClg->amountProfessor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����������:  %d", ptClg->amountAssocProfessor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      �о�������:  %d", ptClg->amountBachelor);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");
        SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
        return;
    }

    while (ptClg)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptClg->clgName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���:    %s", ptClg->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��ʦ����:    %d", ptClg->amountTeacher);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��������:    %d", ptClg->amountProfessor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����������:  %d", ptClg->amountAssocProfessor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      �о�������:  %d", ptClg->amountBachelor);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");
        ptClg = ptClg->nextCollege;
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * Ŀ��:��ʾ�ض���������
 * ����
 *      pScrList �ض�������������ͷ
 */
void showSpecificPaperList(struct Paper* pSrcList)
{
    ptPaper = pSrcList;
    if (ptPaper == NULL)
    {
        MessageBox(hWnd, "û�ҵ�ָ����Ϣ", "��ʾ", MB_ICONERROR);
        return;
    }

    SendMessage(listHwnd, LB_RESETCONTENT, 0, 0);
    SendMessage(listHwnd, WM_SETREDRAW, FALSE, 0);

    char str[100];
    while (ptPaper)
    {
        ListBox_AddString(listHwnd, " ");
        wsprintf(str, "    %s", ptPaper->paperTitle);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��һ����:    %s", ptPaper->paperAuthor);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ��У���:    %s", ptPaper->unyCode);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����Ժϵ:    %s", ptPaper->clgName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����    :    %s", ptPaper->journalName);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ����    :    %s", ptPaper->journalIssue);
        ListBox_AddString(listHwnd, str);
        wsprintf(str, "      ���ķ���:    %c", ptPaper->paperLevel);
        ListBox_AddString(listHwnd, str);
        ListBox_AddString(listHwnd, " ");
        ptPaper = ptPaper->nextPaper;
    }
    SendMessage(listHwnd, WM_SETREDRAW, TRUE, 0);
}

/**
 * ��Ŀ��:ָ����ComboBox�����з��͸�У�б�
 * ����
 *      hComBox ָ����ComboBox����
 */
void sendUnyListToCB (HWND hComBox)
{
    ptUny = getListHead();
    char str[100];
    SendMessage(hComBox, WM_SETREDRAW, FALSE, 0);
    while (ptUny)
    {
        wsprintf(str, "  %s - %s", ptUny->unyCode, ptUny->unyName);
        ComboBox_AddString(hComBox, str);
        ptUny = ptUny->nextUniversity;
    }

    SendMessage(hComBox, WM_SETREDRAW, TRUE, 0);
}

/**
 * ��Ŀ��:ָ����ComboBox�����з���Ժϵ�б�
 * ����
 *      hComBox ָ����ComboBox����
 *      ppUny Ժϵ������У�ڵ�
 */
void sendClgListToCB(HWND hComBox, struct University * ppUny)
{
    struct College *ppClg = ppUny->headCollege;
    SendMessage(hComBox, CB_RESETCONTENT, 0, 0);
    SendMessage(hComBox, WM_SETREDRAW, FALSE, 0);
    while (ppClg)
    {
        ComboBox_AddString(hComBox, ppClg->clgName);
        ppClg = ppClg->nextCollege;
    }
    SendMessage(hComBox, WM_SETREDRAW, TRUE, 0);
}









