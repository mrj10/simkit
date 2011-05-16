#!/bin/bash

LOGFILE=build.log

echo "Running simkit build scripts" | tee -a ${LOGFILE};
echo "Generating local information..." | tee -a ${LOGFILE};
aclocal > ${LOGFILE} 2>&1;
echo "Running autoheader..." | tee -a ${LOGFILE};
autoheader >> ${LOGFILE} 2>&1;
echo "Running autoconf..." | tee -a ${LOGFILE};
autoconf >> ${LOGFILE} 2>&1;
echo "Running automake -a ..." | tee -a ${LOGFILE};
automake -a >> ${LOGFILE} 2>&1;
mkdir -p build;
pushd build >/dev/null 2>&1;
echo "Running configure script..." | tee -a ${LOGFILE};
../configure 2>&1 | tee -a ${LOGFILE};
echo "Building in ./build" | tee -a ${LOGFILE};
make -j 4 2>&1 | tee -a ${LOGFILE};
popd >/dev/null 2>&1;
echo "Done." | tee -a ${LOGFILE};

