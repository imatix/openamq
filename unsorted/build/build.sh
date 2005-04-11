#!/bin/sh
#
# Autobuilder script for OpenAMQ
# Author: mato@imatix.com
#

# Logging function
carp()
{
    echo "$@" 1>&2
    [ -n "${build_log}" ] && echo "$@" >> ${build_log}
}

# Function to abstract away (some) Win32 madness
# Invokes it's arguments, looks in '.' for a script or batch file
# of that name first.
os_invoke()
{
    script="$1"
    if [ "${build_os}" = "Win32" ]; then
        shift
	if [ -f "./${script}.bat" ]; then
	    cmd /c .\\${script} "$@"
	else
	    cmd /c ${script} "$@"
	fi
    else
	if [ -f "./${script}" ]; then
	    sh -c ./"$*"
	else
	    sh -c "$*"
	fi
    fi
}

# Process command line arguments
if [ -z "$1" ]; then
    echo "build E: usage: build CONFIGFILE"
    exit 1
fi
if [ ! -f "$1" ]; then
    echo "build E: $1 does not exist"
    exit 1
fi
build_config=$1

# Get system information
if [ -z "$windir" ]; then
    build_os=`uname`
    build_os_version=`uname -a`
    build_cc=`gcc --version | head -1`
    pkg_extract="tar -xzf"
    pkg_suffix=".tar.gz"
else
    build_os="Win32"
    build_os_version=`cmd /c ver`
    cl 2>build.tmp$$ 1>nul
    build_cc=`head -1 build.tmp$$`
    rm -f build.tmp$$
    pkg_extract="unzip -q"
    pkg_suffix=".zip"
fi

# Check configuration
. ${build_config}
if [ -n "$IBASE" ]; then
    carp "build E: Do not run this script with IBASE set."
    exit 1
fi
if [ ! -d ${BUILD_TOP} ]; then
    carp "build E: ${BUILD_TOP} does not exist."
    exit 1
fi
if [ ! -w ${BUILD_TOP} ]; then
    carp "build E: ${BUILD_TOP} is not writable."
    exit 1
fi
for pkg in ${PACKAGES}; do
    if [ ! -f ${PKGROOT}/${pkg}-src${pkg_suffix} ]; then
	carp "build E: ${PKGROOT}/${pkg}-src${pkg_suffix} does not exist."
	exit 1
    fi
done

# Figure out build date
cvs_checkout_date=`date -u +%Y.%m.%d.%H.%M.%S`
build_date=`echo ${cvs_checkout_date} | sed -e 's!\\.!!g'`

# May have been set in configuration file
export BOOM_MIRROR
export CVSROOT

# Build all models
for model in ${BUILD_MODELS}; do
    build_id="${build_date}-${model}-${build_os}"
    if [ "${build_os}" = "Win32" ]; then
        build_tree="${BUILD_TOP}\\${build_id}"
        build_log="${BUILD_TOP}\\${build_id}.log"
    else
        build_tree="${BUILD_TOP}/${build_id}"
        build_log="${BUILD_TOP}/${build_id}.log"
    fi
    export build_log
    >${build_log}
    mkdir -p ${build_tree} ${build_tree}/src

    carp "build I: Building ${build_id} in ${build_tree}..."
    carp "build I: System: ${build_os_version}"
    carp "build I: Compiler: ${build_cc}"
    carp "build I: Model: ${model}"
    echo "build I: Environment:" >>${build_log}
    if [ "${build_os}" = "Win32" ]; then
	cmd /c set >>${build_log} # 'env' fails mysteriously
    else 
        env >>${build_log}
    fi
    carp "build I: ----------------------------------------"

    # Build all packages
    for pkg in ${PACKAGES}; do
        carp "build I: Building ${pkg}..."
	(
	    set -e
	    BOOM_MODEL=${model}
	    export BOOM_MODEL
	    cd ${build_tree}/src 
	    ${pkg_extract} ${PKGROOT}/${pkg}-src${pkg_suffix}
	    cd ${pkg}
	    os_invoke configure --with-ibase=${build_tree}
            os_invoke boomake build install
	) >>${build_log} 2>&1
	if [ $? -ne 0 ]; then
	    carp "build E: Build of ${pkg} failed"
	    exit 1
	else
	    carp "build I: ${pkg} was built successfully"
	fi
    done

    # Check out and build all CVS products
    export CVSROOT
    for prod in ${PRODUCTS}; do
	carp "build I: Checking out products/${prod}..."
	(
	    set -e
	    cd ${build_tree}/src
	    cvs -Q -z3 checkout -D ${cvs_checkout_date} products/${prod}
	) >>${build_log} 2>&1
	if [ $? -ne 0 ]; then
	    carp "build E: Checkout of ${prod} failed"
	    exit 1
	else
	    carp "build I: Checkout of ${prod} complete"
	fi
	carp "build I: Building ${prod}..."
	(
	    set -e
	    BOOM_MODEL=${model}
	    export BOOM_MODEL
	    IBASE=${build_tree}
	    export IBASE
	    if [ "${build_os}" = "Win32" ]; then
	        PATH=${IBASE}\\bin\;${PATH}
	    else
	        PATH=${IBASE}/bin:${PATH}
	    fi
	    export PATH
	    cd ${build_tree}/src/products/${prod} 
	    os_invoke boom configure
	    os_invoke boomake build install
	) >>${build_log} 2>&1
	if [ $? -ne 0 ]; then
	    carp "build E: Build of ${prod} failed"
	    exit 1
	else
	    carp "build I: ${prod} was built successfully"
	fi
    done

    carp "build I: Build ${build_id} was successful"
    carp "build I: ----------------------------------------"
done
