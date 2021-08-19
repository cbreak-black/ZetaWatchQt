#include "MenuGenerator.hpp"

#include "FormatHelpers.hpp"

#include <ZFSUtils.hpp>
#include <ZFSStrings.hpp>

#include <QtWidgets/QMenu>
#include <QtWidgets/QApplication>
#include <QtCore/QDateTime>

#include <sstream>
#include <chrono>

namespace Zeta
{

MenuGenerator::MenuGenerator() : zetaMenu(new QMenu)
{
	QObject::connect(
	    menu(), &QMenu::aboutToShow, this, &MenuGenerator::aboutToShow);
}

std::chrono::seconds getElapsed(zfs::ScanStat const & scanStat)
{
	auto elapsed = time(0) - scanStat.passStartTime;
	elapsed -= scanStat.passPausedSeconds;
	elapsed = (elapsed > 0) ? elapsed : 1;
	return std::chrono::seconds(elapsed);
}

inline std::string formatTimeRemaining(
    zfs::ScanStat const & scanStat, std::chrono::seconds const & time)
{
	auto bytesRemaining = scanStat.total - scanStat.issued;
	auto issued = scanStat.passIssued;
	if (issued == 0)
		issued = 1;
	auto secondsRemaining = bytesRemaining * time.count() / issued;
	std::stringstream ss;
	ss << std::setfill('0');
	ss << (secondsRemaining / (60 * 60 * 24)) << " days " << std::setw(2)
	   << ((secondsRemaining / (60 * 60)) % 24) << ":" << std::setw(2)
	   << ((secondsRemaining / 60) % 60) << ":" << std::setw(2)
	   << (secondsRemaining % 60);
	return ss.str();
}

void createScrubMenu(zfs::ZPool & pool, QMenu * vdevMenu)
{
	// Scrub
	auto scrub = pool.scanStat();
	auto startDate = QDateTime::fromSecsSinceEpoch(scrub.scanStartTime);
	auto endDate = QDateTime::fromSecsSinceEpoch(scrub.scanEndTime);
	auto startStr = startDate.toString();
	auto endStr = endDate.toString();

	QString scanLine0;
	switch (scrub.state)
	{
	case zfs::ScanStat::stateNone:
	{
		scanLine0 = "Never scrubed";
		break;
	}
	case zfs::ScanStat::scanning:
	{
		scanLine0 = QString("Last scrub from %1 is still in progress");
		scanLine0 = scanLine0.arg(startStr);
		break;
	}
	case zfs::ScanStat::finished:
	{
		scanLine0 = QString("Last scrub from %1 to %2 finished successfully");
		scanLine0 = scanLine0.arg(startStr).arg(endStr);
		break;
	}
	case zfs::ScanStat::canceled:
	{
		scanLine0 = QString("Last scrub from %1 to %2 was canceled");
		scanLine0 = scanLine0.arg(startStr).arg(endStr);
		break;
	}
	}
	vdevMenu->addAction(scanLine0);

	if (scrub.state == zfs::ScanStat::scanning)
	{
		// Scan Stats
		auto elapsed = getElapsed(scrub);
		if (scrub.passPauseTime != 0)
		{
			auto pauseDate = QDateTime::fromSecsSinceEpoch(scrub.passPauseTime);
			auto pauseStr = pauseDate.toString();
			QString scanLinePaused("Scrub Paused since %1");
			scanLinePaused = scanLinePaused.arg(pauseStr);
			vdevMenu->addAction(scanLinePaused);
		}
		QString scanLine1 = "%1 scanned at %2, %3 issued at %4";
		scanLine1 =
		    scanLine1.arg(QString::fromStdString(formatBytes(scrub.scanned)))
		        .arg(QString::fromStdString(
		            formatRate(scrub.passScanned, elapsed)));
		scanLine1 =
		    scanLine1.arg(QString::fromStdString(formatBytes(scrub.issued)))
		        .arg(QString::fromStdString(
		            formatRate(scrub.passIssued, elapsed)));
		QString scanLine2 = "%1 total, %2 % done, %3 remaining, %4 errors";
		scanLine2 =
		    scanLine2.arg(QString::fromStdString(formatBytes(scrub.total)))
		        .arg(
		            QString::number(100.0 * scrub.issued / scrub.total, 'g', 2))
		        .arg(
		            QString::fromStdString(formatTimeRemaining(scrub, elapsed)))
		        .arg(scrub.errors);
		vdevMenu->addAction(scanLine1);
		vdevMenu->addAction(scanLine2);
	}
}

std::string formatStatus(zfs::ZFileSystem const & fs)
{
	std::string formated = fs.name();
	formated += " (";
	if (fs.mounted())
		formated += u8"📌";
	else
		formated += u8"🕳";
	switch (fs.keyStatus())
	{
	case zfs::ZFileSystem::KeyStatus::none:
		break;
	case zfs::ZFileSystem::KeyStatus::unavailable:
		formated += u8"🔒";
		break;
	case zfs::ZFileSystem::KeyStatus::available:
		formated += u8"🔑";
		break;
	}
	auto [encRoot, isRoot] = fs.encryptionRoot();
	if (isRoot)
		formated += u8"🎁";
	formated += u8")";
	return formated;
}

std::string formatErrorStat(zfs::VDevStat stat, bool emoji)
{
	std::stringstream status;
	if (emoji)
	{
		status << zfs::emoji_vdev_state_t(stat.state, stat.aux);
	}
	else
	{
		status << zfs::describe_vdev_state_t(stat.state, stat.aux);
	}
	if (stat.errorRead == 0 && stat.errorWrite == 0 && stat.errorChecksum == 0)
	{
		status << ", No Errors";
	}
	else
	{
		if (stat.errorRead > 0)
			status << ", " << stat.errorRead << " Read Errors";
		if (stat.errorWrite > 0)
			status << ", " << stat.errorWrite << " Write Errors";
		if (stat.errorChecksum > 0)
			status << ", " << stat.errorChecksum << " Checksum Errors";
	}
	return status.str();
}

void addVdev(
    zfs::ZPool const & pool,
    zfs::NVList const & device,
    QMenu * menu,
    unsigned indent = 0)
{
	// Menu Item
	auto stat = zfs::vdevStat(device);
	auto statStr = formatErrorStat(stat, true);
	menu->addAction(QString("%1%2 (%3)")
	                    .arg(QString(indent, '\t'))
	                    .arg(QString::fromStdString(pool.vdevName(device)))
	                    .arg(QString::fromStdString(statStr)));
}

void MenuGenerator::aboutToShow()
{
	zetaMenu->clear();
	zfs::LibZFSHandle zfs;
	for (auto && pool : zfs.pools())
	{
		auto poolTitle = QString("%1 %2")
		                     .arg(pool.name())
		                     .arg(zfs::emoji_pool_status_t(pool.status()));

		QMenu * poolMenu = new QMenu(poolTitle);
		createScrubMenu(pool, poolMenu);
		poolMenu->addSeparator();
		// VDevs
		for (auto && vdev : pool.vdevs())
		{
			// VDev
			addVdev(pool, vdev, poolMenu, 0);
			// Children
			for (auto && device : zfs::vdevChildren(vdev))
			{
				addVdev(pool, device, poolMenu, 1);
			}
		}
		// Caches
		auto caches = pool.caches();
		if (caches.size() > 0)
		{
			poolMenu->addAction("cache");
			for (auto && cache : caches)
			{
				addVdev(pool, cache, poolMenu, 1);
			}
		}
		poolMenu->addSeparator();
		// Filesystems
		for (auto const & fs : pool.allFileSystems())
		{
			auto fsState = formatStatus(fs);
			poolMenu->addAction(QString::fromStdString(fsState));
		}
		zetaMenu->addMenu(poolMenu);
	}

	zetaMenu->addSeparator();
	zetaMenu->addAction("&Quit", &QApplication::quit);
}

QMenu * MenuGenerator::menu()
{
	return zetaMenu.get();
}

} // namespace Zeta