#include "MenuGenerator.hpp"

#include <QtWidgets/QApplication>
#include <QtWidgets/QSystemTrayIcon>

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);

	Zeta::MenuGenerator zeta;

	QSystemTrayIcon trayIcon(QIcon(":/ZetaIcon-Tray.png"));
	trayIcon.setContextMenu(zeta.menu());
	trayIcon.show();

	return app.exec();
}
