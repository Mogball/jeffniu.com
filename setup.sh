#/bin/bash
command -v npm > /dev/null 2>&1 || {
    echo >&2 "Please install npm";
    exit 1;
}

has_libboost="$(dpkg -l | grep libboost-all-dev 2>&1)"
if [ "" = "$has_libboost" ]; then
    echo "Installing libboost-all-dev"
    sudo apt install libboost-all-dev
fi
has_openssl="$(dpkg -l | grep libssl-dev 2>&1)"
if [ "" = "$has_openssl" ]; then
    echo "Installing libssl-dev"
    sudo apt install libssl-dev
fi

command -v postcss > /dev/null 2>&1 || {
    echo "Installing postcss and cssnano"
    sudo npm install postcss-cli --global
}
command -v html-minifier > /dev/null 2>&1 || {
    echo "Installing html-minifier"
    sudo npm install html-minifier --global
}
command -v purifycss > /dev/null 2>&1 || {
    echo "Installing purify-css"
    sudo npm install purify-css --global
}
