set -x #echo on

export PROJECT_ROOT="/Users/vshevchyk/projects/XCloudApp"
export QT_PATH="/Users/vshevchyk/Qt/5.10.1"

read version_str <<< 0.9.0
echo "Version built: $version_str"

cd "$PROJECT_ROOT"
if [ $? -ne 0 ]; then
    echo "Failed to cd purk"
    exit $?
fi

git pull
if [ $? -ne 0 ]; then
    echo "Failed to git pull"
    exit $?
fi

rm -fr build && mkdir build && cd build&& mkdir release && cd release

$QT_PATH/clang_64/bin/qmake ../../
if [ $? -ne 0 ]; then
    echo "Failed to qmake"
    exit $?
fi

make
if [ $? -ne 0 ]; then
    echo "Failed to make"
    exit $?
fi

$QT_PATH/clang_64/bin/macdeployqt X\ Cloud.app
if [ $? -ne 0 ]; then
    echo "Failed to macdeployqt X Cloud.app"
    exit $?
fi

cp ../../resources/logo.icns X\ Cloud.app/Contents/Resources
if [ $? -ne 0 ]; then
    echo "Failed to cp logo.icns to resources"
    exit $?
fi

echo "############### Prepearing archive... ################"
mkdir package_folder
if [ $? -ne 0 ]; then
    echo "Failed to zip app"
    exit 1
fi

ln -s /Applications package_folder/Applications 
if [ $? -ne 0 ]; then
    echo "Failed to copy applications link"
    exit 1
fi

mv X\ Cloud.app package_folder
if [ $? -ne 0 ]; then
    echo "Failed to top app package"
    exit 1
fi


hdiutil create -format UDZO -srcfolder package_folder -volname XCloud "XCloud-macos-x64-$version_str.dmg"
if [ $? -ne 0 ]; then
    echo "Failed to create dmg"
    exit 1
fi

mv XCloud-macos-x64-$version_str.dmg ../
mv -r X\ Cloud.app ../

#zip -r -y "purk-macos-x64-v0.2.0.zip" qt-purk.app
#if [ $? -ne 0 ]; then
#    echo "Failed to zip app"
#    exit $?
# fi

cd ..
echo "Build success"

