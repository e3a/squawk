/**
@mainpage Squawk Media Server
@section TOC Table of Contents
<hr/>

-# @ref TOC
-# @ref Description
-# @ref License
-# @ref Dependencies
-# @ref Build
\n
@section Description Description

@section License License

@section Dependencies Dependencies

Squawk server and client is built using the following 3rd party software:

@subsection Native dependencies

Native libraries:

@subsection Web Web dependencies

Libraties for the web page:

<ul>
<li><a href="https://angularjs.org/">angularjs</a>&nbsp;Google javascript framework</li>
<li><a href="http://getbootstrap.com/">bootstrap</a>&nbsp;Twitter css templates</li>
<li><a href="http://angular-ui.github.io/bootstrap/">angular-UI-Bootstrap</a>&nbsp;Twitter Bootstrap written natively in AngularJS</li>
<li><a href="https://github.com/dbtek/angular-aside">angular-aside</a>&nbsp;Off canvas side menu to use with ui-bootstrap.</li>
<li><a href="https://github.com/michaelbromley/angularUtils/tree/master/src/directives/pagination">angular-pagination</a>&nbsp;Content pagination module.</li>
</ul>

@section Build Build the Server

\n
install dependencies:

apt-get install \ 
  git \ 
  cmake \ 
  libsqlite3-dev \
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
