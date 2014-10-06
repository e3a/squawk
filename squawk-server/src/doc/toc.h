/**
@mainpage Squawk Media Server
\n
The Squawk media server .................
aesfr
asf
\n
<hr/>
@section Table of Contents
\n
-# @ref Description
-# @ref License
-# @ref Build
\n
@section Build Build the Server
\n
install dependencies:

apt-get install \ 
  git \ 
  cmake \ 
  libsqlite3-dev \
  liblog4cxx10-dev \
  libasio-dev \
  libtinyxml-dev \
  libboost-dev \
  libboost-system-dev \
  libboost-filesystem-dev \
  libboost-thread-dev \
  libboost-program-options-dev \
  libflac++-dev \
  libid3-3.8.3-dev \
  libimlib2-dev \
  libpcre++-dev \
  libgtest-dev \
  doxygen
 libmagic-dev
 libswscale-dev
 libsdl1.2-dev
 
gtest needs to be installed manually:

cd /usr/src/gtest
sudo cmake .
sudo make
sudo mv libg* /usr/lib/

build the source code:
  cd $HOME
  git clone https://github.com/e3a/squawk-server.git
  cd squawk-server
  mkdir build
  cmake ..
  make
  make test 
  make doc  
*/