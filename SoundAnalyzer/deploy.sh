cp -f ../SoundAnalyzer/build-SoundAnalyzer-Qt_5_5_1_gcc_64-Vydání/SoundAnalyzer SoundAnalyzer
rm -R -f lib
rm -R -f qml
rm -R -f plugins
cp -R ../../dpdata/DeployData/lib lib
cp -R ../../dpdata/DeployData/qml qml
cp -R ../../dpdata/DeployData/plugins plugins
cp ../Source/SoundAnalyzer/licence.txt licence.txt
export PATH=~/Qt5.5.1/5.5/gcc_64/bin:$PATH
./linuxdeployqt SoundAnalyzer -qmldir=../Source/SoundAnalyzer
./linuxdeployqt SoundAnalyzer -qmldir=../Source/SoundAnalyzer -no-plugins



