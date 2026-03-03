#Clean old builds
set -e

if [[ "$1" == "--clean" ]]; then
    echo "Cleaning build directories..."
    rm -rf build-asan build-tsan
    exit 0
fi

BUILD_TYPE="asan"

if [[ "$1" == "tsan" ]]; then
    BUILD_TYPE="tsan"
fi

BUILD_DIR="build-${BUILD_TYPE}"

mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

cmake -DSANITIZER=${BUILD_TYPE} ..
cmake --build . -j$(nproc)

ctest --output-on-failure
# chmod +x build.sh 