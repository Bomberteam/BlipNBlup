#LucKey Productions Blip 'n Blup install script for Xubuntu
sudo apt-get update
sudo apt-get install -y git qtcreator
sudo apt-get update
sudo apt-get upgrade -y

if [ ! -d ~/LucKeyProductions ]
then
  mkdir ~/LucKeyProductions
fi

cd ~/LucKeyProductions

if [ ! -d Urho3D ]
then
  git clone https://github.com/Urho3D/Urho3D
  cd Urho3D
  cmake .
  make
  ./cmake_generic.sh
  cd ..
fi

if [ ! -d BlipNBlup ]
then
  git clone https://github.com/LucKeyProductions/BlipNBlup
  cd BlipNBlup
  ln -s ../Urho3D/
  cd ..

  mkdir build-BlipNBlup-Desktop-Debug
  cd build-BlipNBlup-Desktop-Debug
  ln -s ../Urho3D/bin/Data
  ln -s ../Urho3D/bin/CoreData
  ln -s ../BlipNBlup/Resources
  cd ..

  mkdir build-BlipNBlup-Desktop-Release
  cd build-BlipNBlup-Desktop-Release
  ln -s ../Urho3D/bin/Data
  ln -s ../Urho3D/bin/CoreData
  ln -s ../BlipNBlup/Resources
  cd ..

fi

cd BlipNBlup
qtcreator BlipNBlup.pro
