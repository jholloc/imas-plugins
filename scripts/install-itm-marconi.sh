PLUGINS_HOME=/gw/swimas/extra/uda/plugins/1.2.0/AL/4.8.3

cp $UDA_HOME/etc/plugins/udaPlugins.conf $PLUGINS_HOME/etc/plugins/udaPlugins.conf
cp $UDA_HOME/lib/plugins/* $PLUGINS_HOME/lib/plugins

./build/script/activate-plugins.sh

mkdir $PLUGINS_HOME/etc/exp2imas
cp -r source/exp2imas/mappings $PLUGINS_HOME/etc/exp2imas/mappings/

mkdir $PLUGINS_HOME/etc/imas_mapping
cp source/imas_mapping/machines.txt $PLUGINS_HOME/etc/imas_mapping
