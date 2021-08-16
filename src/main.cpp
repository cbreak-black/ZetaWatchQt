#include <ZFSUtils.hpp>

#include <QtWidgets/QApplication>
#include <QtWidgets/QSystemTrayIcon>
#include <QtWidgets/QMenu>

QMenu * createZetaMenu()
{
	QMenu * zetaMenu = new QMenu();

	zfs::LibZFSHandle zfs;
	for (auto const & pool : zfs.pools())
	{
		QMenu * poolMenu = new QMenu(pool.name());
		for (auto const & fs : pool.allFileSystems())
		{
			poolMenu->addAction(fs.name());
		}
		zetaMenu->addMenu(poolMenu);
	}

	return zetaMenu;
}

int main(int argc, char ** argv)
{
	QApplication app(argc, argv);

	QMenu * zetaMenu = createZetaMenu();

	zetaMenu->addSeparator();
	zetaMenu->addAction("&Quit", &QApplication::quit);

	QSystemTrayIcon trayIcon(QIcon(":/ZetaIcon-Tray.png"));
	trayIcon.setContextMenu(zetaMenu);
	trayIcon.show();

	return app.exec();
}
