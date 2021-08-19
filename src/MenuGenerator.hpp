#ifndef ZETA_MENU_GENERATOR_HPP
#define ZETA_MENU_GENERATOR_HPP

#include <QtWidgets/QMenu>

namespace Zeta
{

class MenuGenerator : public QObject
{
public:
	MenuGenerator();

	void aboutToShow();

	QMenu * menu();

private:
	QScopedPointer<QMenu> zetaMenu;
};

} // namespace Zeta

#endif