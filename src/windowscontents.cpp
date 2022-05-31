#include "windowscontents.h"

HWND GetWindowHandleByPID(const DWORD targetPID)
{
    HWND hWnd = GetTopWindow(NULL);
    do {
        if (GetWindowLong(hWnd, GWL_HWNDPARENT) != 0 || !IsWindowVisible(hWnd))
            continue;

        DWORD getPID;
        GetWindowThreadProcessId(hWnd, &getPID);

        if (targetPID == getPID)
            return hWnd;

    } while((hWnd = GetNextWindow(hWnd, GW_HWNDNEXT)) != NULL);

    return NULL;
}


QWidget* getWidgetFromProcess(QProcess *process)
{
    if(!process) return nullptr;

    HWND winHandle = GetWindowHandleByPID(process->processId());

    return QWidget::createWindowContainer(QWindow::fromWinId(WId(winHandle)));
}







