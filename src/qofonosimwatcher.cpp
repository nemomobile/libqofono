/****************************************************************************
**
** Copyright (C) 2015 Jolla Ltd.
** Contact: slava.monich@jolla.com
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "qofonosimwatcher.h"
#include "qofonomanager.h"

class QOfonoSimWatcher::Private : public QObject {
    Q_OBJECT

public:
    Private(QOfonoSimWatcher *parent);

    QOfonoSimWatcher *watcher;
    QSharedPointer<QOfonoManager> ofono;
    QHash<QString, QOfonoSimManager::SharedPointer> allSims;
    QList<QOfonoSimManager::SharedPointer> presentSims;
    bool valid;

private Q_SLOTS:
    void onOfonoAvailableChanged();
    void updateValid();
    void updateModems();
    void updateSims();
};

QOfonoSimWatcher::Private::Private(QOfonoSimWatcher *parent) :
    QObject(parent),
    watcher(parent),
    ofono(QOfonoManager::instance()),
    valid(false)
{
    connect(ofono.data(), SIGNAL(availableChanged(bool)), SLOT(onOfonoAvailableChanged()));
    connect(ofono.data(), SIGNAL(modemsChanged(QStringList)), SLOT(updateModems()));
    if (ofono->available()) {
        onOfonoAvailableChanged();
    }
}

void QOfonoSimWatcher::Private::onOfonoAvailableChanged()
{
    if (ofono->available()) {
        updateModems();
        updateValid();
    } else {
        updateValid();
        allSims.clear();
        if (!presentSims.isEmpty()) {
            presentSims.clear();
            Q_EMIT watcher->presentSimListChanged();
            Q_EMIT watcher->presentSimCountChanged();
        }
    }
}

void QOfonoSimWatcher::Private::updateModems()
{
    QStringList oldModems = allSims.keys();
    QStringList newModems = ofono->modems();
    oldModems.sort();
    newModems.sort();
    if (oldModems != newModems) {
        const int oldCount = oldModems.count();
        const int newCount = newModems.count();
        int i;
        for (i=0; i<oldCount; i++) {
            QString path(oldModems.at(i));
            if (!newModems.contains(path)) {
                // This modem is no more
                allSims.take(path)->disconnect(this);
            }
        }
        for (i=0; i<newCount; i++) {
            QString path(newModems.at(i));
            if (!allSims.contains(path)) {
                // This is a new modem
                QOfonoSimManager *sim = new QOfonoSimManager();
                sim->fixModemPath(path);
                connect(sim, SIGNAL(validChanged(bool)), SLOT(updateSims()));
                connect(sim, SIGNAL(presenceChanged(bool)), SLOT(updateSims()));
                allSims.insert(path, QOfonoSimManager::SharedPointer(sim));
            }
        }
        updateSims();
    }
}

void QOfonoSimWatcher::Private::updateSims()
{
    QList<QOfonoSimManager::SharedPointer> sims;
    QStringList modems = allSims.keys();
    modems.sort();
    int i, n = modems.count();
    for (i=0; i<n; i++) {
        QOfonoSimManager::SharedPointer sim = allSims.value(modems.at(i));
        if (sim->isValid() && sim->present()) {
            sims.append(sim);
        }
    }
    if (sims.count() != presentSims.count()) {
        presentSims = sims;
        Q_EMIT watcher->presentSimListChanged();
        Q_EMIT watcher->presentSimCountChanged();
    } else {
        n = sims.count();
        for (i=0; i<n; i++) {
            if (sims.at(i).data() != presentSims.at(i).data()) {
                presentSims = sims;
                Q_EMIT watcher->presentSimListChanged();
                break;
            }
        }
    }
    updateValid();
}

void QOfonoSimWatcher::Private::updateValid()
{
    // This object is valid if QOfonoManager and all SIM managers are valid.
    bool isValid = ofono->available();
    if (isValid) {
        QList<QOfonoSimManager::SharedPointer> sims = allSims.values();
        const int n = sims.count();
        for (int i=0; i<n && isValid; i++) {
            isValid = sims.at(i)->isValid();
        }
    }
    if (valid != isValid) {
        valid = isValid;
        Q_EMIT watcher->validChanged();
    }
}

QOfonoSimWatcher::QOfonoSimWatcher(QObject *parent) :
    QObject(parent),
    d_ptr(new Private(this))
{
}

QOfonoSimWatcher::~QOfonoSimWatcher()
{
    // d_ptr is a child object, it gets deleted automatically
}

bool QOfonoSimWatcher::isValid() const
{
    return d_ptr->valid;
}

int QOfonoSimWatcher::presentSimCount() const
{
    return d_ptr->presentSims.count();
}

QList<QOfonoSimManager::SharedPointer> QOfonoSimWatcher::presentSimList() const
{
    return d_ptr->presentSims;
}

#include "qofonosimwatcher.moc"
