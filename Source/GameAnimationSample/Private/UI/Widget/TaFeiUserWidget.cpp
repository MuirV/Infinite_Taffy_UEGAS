

#include "UI/Widget/TaFeiUserWidget.h"

void UTaFeiUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet(); // 通知蓝图
}