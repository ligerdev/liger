/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#ifndef DESIGNFILEWIZARD_H
#define DESIGNFILEWIZARD_H

#include <designer/designer_global.h>
#include <designer/designerplugin.h>
#include <coreplugin/dialogs/iwizard.h>
#include <coreplugin/generatedfile.h>
#include <coreplugin/dialogs/iwizard.h>
#include <extensionsystem/iplugin.h>
#include <QSharedDataPointer>
#include <QList>


QT_BEGIN_NAMESPACE
class QIcon;
class QWizard;
class QWizardPage;
class QDebug;
QT_END_NAMESPACE

namespace Utils {
    class Wizard;
}

namespace Designer {

class IDesigner;
class IFileWizardExtension;

class DesignFileWizardParameterData;
struct DesignFileWizardPrivate;

class DESIGNER_EXPORT DesignFileWizardParameters
{
public:
    explicit DesignFileWizardParameters(Core::IWizard::WizardKind kind = Core::IWizard::FileWizard);
    DesignFileWizardParameters(const DesignFileWizardParameters &);
    DesignFileWizardParameters &operator=(const DesignFileWizardParameters&);
   ~DesignFileWizardParameters();

    void clear();

    Core::IWizard::WizardKind kind() const;
    void setKind(Core::IWizard::WizardKind k);

    QIcon icon() const;
    void setIcon(const QIcon &icon);

    QString description() const;
    void setDescription(const QString &description);

    QString displayName() const;
    void setDisplayName(const QString &name);

    QString id() const;
    void setId(const QString &id);

    QString category() const;
    void setCategory(const QString &category);

    QString displayCategory() const;
    void setDisplayCategory(const QString &trCategory);

    Core::FeatureSet requiredFeatures() const;
    void setRequiredFeatures(Core::FeatureSet features);

    Core::IWizard::WizardFlags flags() const;
    void setFlags(Core::IWizard::WizardFlags flags);

    QString descriptionImage() const;
    void setDescriptionImage(const QString &path);

private:
    QSharedDataPointer<DesignFileWizardParameterData> m_d;
};

DESIGNER_EXPORT QDebug operator<<(QDebug d, const DesignFileWizardParameters &);

class DESIGNER_EXPORT WizardDialogParameters
{
public:
    typedef QList<QWizardPage *> WizardPageList;

    enum DialogParameterEnum {
        ForceCapitalLetterForFileName = 0x01
    };
    Q_DECLARE_FLAGS(DialogParameterFlags, DialogParameterEnum)

    explicit WizardDialogParameters(const QString &defaultPath, const WizardPageList &extensionPages,
                                    const QString &platform, const Core::FeatureSet &requiredFeatures,
                                    DialogParameterFlags flags,
                                    QVariantMap extraValues)
        : m_defaultPath(defaultPath),
          m_extensionPages(extensionPages),
          m_selectedPlatform(platform),
          m_requiredFeatures(requiredFeatures),
          m_parameterFlags(flags),
          m_extraValues(extraValues)
    {}

    QString defaultPath() const
    { return m_defaultPath; }

    WizardPageList extensionPages() const
    { return m_extensionPages; }

    QString selectedPlatform() const
    { return m_selectedPlatform; }

    Core::FeatureSet requiredFeatures() const
    { return m_requiredFeatures; }

    DialogParameterFlags flags() const
    { return m_parameterFlags; }

    QVariantMap extraValues() const
    { return m_extraValues; }

private:
    QString m_defaultPath;
    WizardPageList m_extensionPages;
    QString m_selectedPlatform;
    Core::FeatureSet m_requiredFeatures;
    DialogParameterFlags m_parameterFlags;
    QVariantMap m_extraValues;
};

class DESIGNER_EXPORT DesignFileWizard : public Core::IWizard
{
    Q_OBJECT

public:
    virtual ~DesignFileWizard();

    // IWizard
    virtual WizardKind kind() const;
    virtual QIcon icon() const;
    virtual QString description() const;
    virtual QString displayName() const;
    virtual QString id() const;

    virtual QString category() const;
    virtual QString displayCategory() const;

    virtual QString descriptionImage() const;

    virtual void runWizard(const QString &path, QWidget *parent, const QString &platform, const QVariantMap &extraValues);
    virtual Core::FeatureSet requiredFeatures() const;
    virtual WizardFlags flags() const;

    static QString buildFileName(const QString &path, const QString &baseName, const QString &extension);
    static void setupWizard(QWizard *);
    static void applyExtensionPageShortTitle(Utils::Wizard *wizard, int pageId);

protected:
    typedef QList<QWizardPage *> WizardPageList;

    explicit DesignFileWizard(const DesignFileWizardParameters &parameters, QObject *parent = 0);

    DesignFileWizardParameters baseFileWizardParameters() const;

    virtual QWizard *createWizardDialog(QWidget *parent,
                                        const WizardDialogParameters &wizardDialogParameters) const = 0;

    virtual Core::GeneratedFiles generateFiles(const QWizard *w,
                                         QString *errorMessage) const = 0;

    virtual bool writeFiles(const Core::GeneratedFiles &files,
                            QString *errorMessage);

    virtual bool postGenerateFiles(const QWizard *w,
                                   const Core::GeneratedFiles &l,
                                   QString *errorMessage);

    static QString preferredSuffix(const QString &mimeType);

    enum OverwriteResult { OverwriteOk,  OverwriteError,  OverwriteCanceled };
    OverwriteResult promptOverwrite(Core::GeneratedFiles *files,
                                    QString *errorMessage) const;
    static bool postGenerateOpenDesigners(const Core::GeneratedFiles &l, QString *errorMessage = 0);

private:
    DesignFileWizardPrivate *d;
};

class DESIGNER_EXPORT StandardDesignWizard : public DesignFileWizard
{
    Q_OBJECT

public:
    explicit StandardDesignWizard(const DesignFileWizardParameters &parameters, QObject *parent = 0);
    virtual QWizard *createWizardDialog(QWidget *parent, const WizardDialogParameters &wizardDialogParameters) const;
    virtual Core::GeneratedFiles generateFiles(const QWizard *w, QString *errorMessage) const;
    virtual Core::GeneratedFiles generateFilesFromPath(const QString &path, const QString &name,
                                                 QString *errorMessage) const;

private:
    const QString m_mimeType;
    const QString m_suggestedFileName;
};

template <class WizardClass>
QList<WizardClass*> createMultipleDesignFileWizardInstances(const QList<DesignFileWizardParameters> &parametersList, ExtensionSystem::IPlugin *plugin)
{
    QList<WizardClass*> list;
    foreach (const DesignFileWizardParameters &parameters, parametersList) {
        WizardClass *wc = new WizardClass(parameters, 0);
        plugin->addAutoReleasedObject(wc);
        list << wc;
    }
    return list;
}

} // namespace Core

Q_DECLARE_OPERATORS_FOR_FLAGS(Core::GeneratedFile::Attributes)
Q_DECLARE_OPERATORS_FOR_FLAGS(Designer::WizardDialogParameters::DialogParameterFlags)

#endif // DESIGNFILEWIZARD_H
