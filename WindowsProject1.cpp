#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <ObjIdl.h>
#include <gdiplus.h>
#include <gdiplusheaders.h>
using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

// 定义最大宽度
#define MAX_WIDTH 150
// 定义最大高度
#define MAX_HEIGHT 200

using namespace std;

// 声明函数，用于在窗口上绘制文本
void Drawtext(HWND hwnd, HDC hdc);

// 窗口过程函数，用于处理窗口消息
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_COMMAND:
        // 如果接收到命令消息，并且wParam的低位字为1
        if (LOWORD(wParam) == 1) {
            // 弹出消息框，表明按钮1被点击
            MessageBox(hwnd, L"Button 1 Clicked!", L"Notification", MB_OK);
        }
        else if (LOWORD(wParam) == 2) {
            // 弹出消息框，表明按钮2被点击
            MessageBox(hwnd, L"Button 2 Clicked!", L"Notification", MB_OK);
        }
        break;
    case WM_DESTROY:
        // 接收到销毁消息时，发送退出消息
        PostQuitMessage(0);
        break;
    case WM_NCHITTEST:
        // 返回值用于表示点击的区域是窗口标题栏
        return HTCAPTION;
    default:
        // 调用默认的窗口过程处理其他消息
        return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

// 定义一个全局变量hinst，用于存储应用程序实例句柄
HINSTANCE hinst;

// 应用程序入口点
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevinstance, PSTR szCmdLine, int iCmdShow) {
    HWND hWnd;
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;

    // 初始化GDI+库
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    hinst = GetModuleHandle(NULL);

    // 定义窗口类的属性
    WNDCLASS wc = {};
    wc.lpfnWndProc = WndProc; // 窗口过程
    wc.hInstance = hinst; // 窗口实例
    wc.lpszClassName = L"win32"; // 窗口类名

    // 注册窗口类
    RegisterClass(&wc);

    // 创建窗口
    hWnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TOPMOST, // 扩展窗口样式
        L"win32", // 窗口类名
        L"WinSoup", // 窗口标题
        WS_POPUP, // 窗口样式
        0, 0, 1000, 500, // 窗口位置和尺寸
        nullptr, // 父窗口句柄
        nullptr, // 菜单句柄
        hInstance, // 应用程序实例
        nullptr // 附加参数
    );

    // 如果窗口创建失败，返回0退出程序
    if (hWnd == NULL) {
        return 0;
    }

    // 调用Drawtext函数，在窗口上绘制文本
    Drawtext(hWnd, GetDC(hWnd));

    // 显示窗口
    ShowWindow(hWnd, SW_SHOW);

    MSG msg = {};

    // 消息循环，获取和处理消息
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 关闭GDI+库
    GdiplusShutdown(gdiplusToken);
    return 0;
}

// 
// 定义Drawtext函数，用于绘制文本
void Drawtext(HWND hwnd, HDC hdc)
{

    // 字体设定
    
    // const FontFamily* fontFamilyPtr = FontFamily::GenericSansSerif(); // 中文仍然是有衬线的

    FontFamily fontFamily(L"Microsoft Yahei");
    const FontFamily* fontFamilyPtr = &fontFamily;
    Font        font(fontFamilyPtr, 16, FontStyleRegular, UnitPixel); 

    PointF      pointF(30.0f, 10.0f); // 文本绘制起点
    SolidBrush  solidBrush(Color(255, 255, 255, 255)); // 笔刷，定义文本颜色

    Bitmap softwareBitmap(MAX_WIDTH, MAX_HEIGHT, PixelFormat32bppARGB); // 位图对象
    Graphics g(&softwareBitmap); // 图形对象
    g.Clear(Gdiplus::Color(128, 0x66, 0xcc, 0xff)); // 填充背景颜色
    g.SetTextRenderingHint(TextRenderingHintAntiAlias); // 提高文本的显示质量
    g.DrawString(L"Hello \n World\n你好", -1, &font, pointF, &solidBrush); // 绘制字符串

    HBITMAP bmp;
    softwareBitmap.GetHBITMAP(Color(0, 0, 0, 0), &bmp); // 获取位图句柄

    HDC memdc = CreateCompatibleDC(hdc); // 创建内存设备上下文
    HGDIOBJ original = SelectObject(memdc, bmp); // 选择位图进入内存设备上下文

    BLENDFUNCTION blend = { 0 }; // 混合函数
    blend.BlendOp = AC_SRC_OVER; // 混合操作
    blend.SourceConstantAlpha = 255; // 源常量Alpha值
    blend.AlphaFormat = AC_SRC_ALPHA; // Alpha格式
    POINT ptLocation = { 200, 300 }; // 位图位置
    SIZE szWnd = { MAX_WIDTH, MAX_HEIGHT }; // 位图尺寸
    POINT ptSrc = { 0, 0 }; // 源起点
    UpdateLayeredWindow(hwnd, hdc, &ptLocation, &szWnd, memdc, &ptSrc, 0, &blend, ULW_ALPHA); // 更新分层窗口
    SelectObject(hdc, original); // 恢复原始对象

    DeleteObject(bmp); // 删除位图对象
    DeleteObject(memdc); // 删除内存设备上下文
}

