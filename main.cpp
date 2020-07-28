#include <QCoreApplication>
#include <QSettings>
#include <QTextStream>
#include <QUrl>
#include <QList>
#include <QStringList>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QTextStream stream(stdout);

    QStringList arguments = a.arguments();
    arguments.removeFirst();
    if (*arguments.begin() != QLatin1String("ensure-shortcuts")) {
        stream << "Requested action: " << *arguments.begin() << endl;
        stream << "ERROR: `ensure-shortcuts` is the only supported action." << endl;
        stream << "Usage: `qtctl ensure-shortcuts <path1>.." << endl;
        return 1;
    }
    arguments.removeFirst();
    if (arguments.length() < 1) {
        stream << "ERROR: `ensure-shortcuts` requires a list of paths to add bookmarks for." << endl;
        stream << "Usage: `qtctl ensure-shortcuts <path1>.." << endl;
        return 1;
    }


    QList<QUrl> requestedUrls;
    for ( const QString& argument : arguments  )
    {
        requestedUrls << QUrl::fromLocalFile(argument);
    }
    requestedUrls.prepend(QUrl(QLatin1String("file:")));
    stream << "INFO: requested urls: " << QUrl::toStringList(requestedUrls).join(" ") << endl;


    QSettings settings(QSettings::UserScope, QLatin1String("QtProject"));
    if (!settings.childGroups().contains(QLatin1String("FileDialog"))) {
        stream << "INFO: FileDialog group doesn't exist, creating one now." << endl;
    }
    settings.beginGroup(QLatin1String("FileDialog"));


    // For some reason, this is a required variable, otherwise the config file is seen as invalid.
    if (!settings.contains(QLatin1String("sidebarWidth"))){
        settings.setValue(QLatin1String("sidebarWidth"), 98);
    }


    if (!settings.contains(QLatin1String("shortcuts"))){
        stream << "INFO: shortcuts key doesn't exist" << endl;
        settings.setValue(QLatin1String("shortcuts"), QUrl::toStringList(requestedUrls));
        // + file:
    } else {
        QList<QUrl> sidebarUrls = QUrl::fromStringList(settings.value(QLatin1String("shortcuts")).toStringList());
        stream << "INFO: Old keys: " << QUrl::toStringList(sidebarUrls).join(" ") << endl;
        foreach (QUrl requestedUrl, requestedUrls) {
            if ( ! sidebarUrls.contains(requestedUrl)) {
                sidebarUrls << requestedUrl;
            }
        }
        settings.setValue(QLatin1String("shortcuts"), QUrl::toStringList(sidebarUrls));
    }

    stream << "INFO: success!" << endl;

    return 0;
}
