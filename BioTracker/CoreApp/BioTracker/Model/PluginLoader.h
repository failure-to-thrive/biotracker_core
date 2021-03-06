/****************************************************************************
  **
  ** This file is part of the BioTracker Framework
  ** by Andreas Jörg
  **
  ****************************************************************************/

#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "Interfaces/IModel/IModel.h"
#include "QPluginLoader"
#include "QStringListModel"
#include "Interfaces/IBioTrackerPlugin.h"
#include "QPointer"

/**
 * The PluginLoader class is a IModel class. It is responsible for managing BioTracker Plugins. 
 * It can load them and it sends a list of all loaded Plugin names to the a selection box in the MainWindow.
 */
class PluginLoader : public IModel {
    Q_OBJECT
  public:
	  explicit PluginLoader(QObject* parent = 0);

	  /**
	  * Loads a file as a QT plugin and reads it's name from the metadata.
	  * It will then be added to the stringlist and is selectable via "loadPluginFromName".
	  * This function does not actually set the plugin instance.
	  */
	  void addToPluginList(QString p);

	  /**
	  * Loads a BioTracker Plugin from a filpaht. It returns true if the Plugin could be loaded, otherwise false.
	  */
	  bool loadPluginFromFilename(QString filename);

	  /**
	  * Loads a plugin from it's name (given in the metadata) instead of the filename.
	  * Only works if it is already known to the pluginloader, i.e. was loaded manually or automatically before or
	  * or has been added to the pluginlist via "addToPluginList"
	  */
	  bool loadPluginFromName(QString name);

	  /**
	  * Returns the Instance of the BioTracker Plugin.
	  */
	  IBioTrackerPlugin* getPluginInstance();

	  /**
	  * Returns a QStrinListModel with the names of all loaded Plugins.
	  */
	  QStringListModel* getPluginMetaData();

	  // Is a plugin loaded or not?
	  bool getIsPluginLoaded();

	  // Gets the name of the currently loaded plugin
	  QString getCurrentPluginName();

      /**
      * Returns a map containing the mapping "plugin name -> filename"
      */
      std::map<QString, QString> const& getPluginMap() const;

private:

    //a map containing the mapping "plugin name -> filename"
	std::map<QString, QString> m_PluginMap;

    //nomen est omen
	bool m_isPluginLoaded;

    //Name of the currently loaded plugin
	QString m_currentPluginName;

    /**
    * Gets the metadata from the currently loaded plugin.
    * Currently this is nothing but the advertised name of the plugin
    */
	QString readMetaDataFromPlugin();

    // The QT object to actually load the plugins
	QPluginLoader* m_PluginLoader;

    // List of all available plugins 
	QStringList m_PluginList;

    // Entire ListModel of the metadata (actually containing all metadata, not only name)
	QStringListModel* m_PluginListModel;


};

#endif // PLUGINLOADER_H
