#ifndef WINDOWSCONTENTS_H
#define WINDOWSCONTENTS_H
#include <Windows.h>
#include <QWidget>
#include <QProcess>
#include <QWindow>

#ifndef GWL_HWNDPARENT
#define GWL_HWNDPARENT -8
#endif




//get window handle from the process Id.
HWND GetWindowHandleByPID(const DWORD targetPID);

//create widget from the process Id. The process needs to have visible window.
QWidget* getWidgetFromProcess(QProcess *process);








#endif // WINDOWSCONTENTS_H
