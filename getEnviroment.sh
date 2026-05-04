cd ..
location_path=`pwd`

for sub_path in `find third-party/stacks -name *.a | awk -F "/" '{print $1"/"$2"/"$3"/"$4}' | uniq`
do
    co_path=${location_path}/${sub_path}/pkgconfig
    PKG_CONFIG_PATH=${co_path}:${PKG_CONFIG_PATH}

done
echo $PKG_CONFIG_PATH