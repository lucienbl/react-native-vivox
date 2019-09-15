printf "Building libraries for Android...\n"
cd android || exit
printf "Deleting old build folders...\n"
rm -rf libs
rm -rf obj
rm -rf src/main/jniLibs/**
ndk-build
cp -r libs/** src/main/jniLibs
printf "Done!\n"
