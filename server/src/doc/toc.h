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

The deveopment build of these libraries have to be installed on the build system:

<ul>
<li><a href="http://www.boost.org">Boost</a>&nbsp;Boost provides free peer-reviewed portable C++ source libraries.</li>
<li><a href="http://think-async.com">Boost Asio</a>&nbsp;Asio is a cross-platform C++ library for network and low-level I/O programming that provides developers with a consistent asynchronous model using a modern C++ approach.</li>
<li><a href=""></a>&nbsp;</li>
<li><a href=""></a>&nbsp;</li>
<li><a href=""></a>&nbsp;</li>
</ul>

@subsection External External Dependencies

<ul>
<li><a href="http://fmtlib.net/latest/index.html">{fmt}</a>&nbsp;fmt (formerly cppformat) is an open-source formatting library. It can be used as a safe alternative to printf or as a fast alternative to C++ IOStreams.</li>
<li><a href="https://github.com/no1msd/mstch">{{mustache}}</a>&nbsp;mstch is a complete implementation of {{mustache}} templates using modern C++. It's compliant with specifications v1.1.3, including the lambda module.</li>
<li><a href="http://easylogging.muflihun.com">Easylogging++</a>&nbsp;Single header only, extremely light-weight high performance logging library for C++11 (or higher) applications.</li>
<li><a href="https://github.com/google/re2">RE2</a>&nbsp;This is the source code repository for RE2, a regular expression library.</li>
</ul>

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
