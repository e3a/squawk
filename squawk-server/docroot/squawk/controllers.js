var items_per_line = 8;
var loaded_lines_start = 0;
var loaded_lines_end = 0;
var last_item = 0;
var items_count = 0;
var loaded_rows = {};
var frag;

var old_start = -1, old_end = -1;

function getImageSize($window) {
  var size = Math.floor( ( $window.innerWidth / items_per_line ) - 4);
  return size;
}
function row_ciel($window, imageSize) {
    var ciel = Math.floor($window.pageYOffset / imageSize);
    return (ciel == 0 ? 0 : ciel - 1);
}
function row_floor($window, imageSize) {
    return ( Math.ceil( ( $window.pageYOffset + $window.innerHeight ) / imageSize ) + 1 );
}
function total_height(count, imageSize) {
    return( 25 + (Math.ceil(count/items_per_line))*(imageSize+2) );
}
function load_rows(element, start, end, imageSize) {
    if( end > old_end || old_end == -1 ) {
        var load_from = old_end + 1;
        old_start = start; old_end = end;
        for( i=load_from; i<=end; i++ ) {
            console.log("load rows:" + start + "(" + old_start + "), " + end + "(" + old_end + ")");
            load_row(frag, i, imageSize);
        }
    }
}
function load_row(element, row, imageSize) {
    console.log("load row:" + row);
    var index = (row) * items_per_line;

//    if ( loaded_rows[row] == undefined ) {
//        loaded_rows[row] = new Array();
        jQuery.ajax({url: "/api/albums?index=" + (row*items_per_line) + "&limit="+items_per_line, async: false, success: function( data ) {
            if( data.count != items_count ) {
                items_count = data.count;
                $('body').height( total_height( data.count, imageSize ) );
            }
            var result_size = data.albums.length;
            for(item=0; item<result_size; item++) {

                var domImg = document.createElement("img");
                domImg.setAttribute("src", '/album/' + data.albums[item].id + '/cover.jpg');
                domImg.setAttribute("style","width:" + imageSize + "px; height:" + imageSize + "px;");
                console.log("add image:"+"#"+((row*items_per_line)+item));
                $("#"+((row*items_per_line)+item)).append(domImg);

//                var albumItem = getAlbumItem( data.albums[item], imageSize, row, item);
//                element.appendChild( albumItem );
//                loaded_rows[row][j] = albumItem;
            }
        }});
//    }
}

function getAlbumItem(album, size, row, col) {
    var domContainer = document.createElement("div");
    domContainer.setAttribute("style", "top: " + row * (size + 2) + "px; left: " + col * (size+2) + "px; width:" + Math.floor(size) + "px; height:" + Math.floor(size) + "px;");
    domContainer.setAttribute("class","box");
//    domContainer.style.top = row * (size + 2);
//    domContainer.style.left = col * (size+2);
//    domContainer.setAttribute("style","width:" + Math.floor(size) + "px; height:" + Math.floor(size) + "px;");

    var domImg = document.createElement("img");
    domImg.setAttribute("src", '/album/' + album.id + '/cover.jpg');
    domImg.setAttribute("style","width:" + Math.floor(size) + "px; height:" + Math.floor(size) + "px;");

    domContainer.appendChild(domImg);

    var domSpan = document.createElement("span");
    domSpan.setAttribute("onclick", 'window.location = \"#/album/' + album.id + '\"');
    domSpan.setAttribute("class","caption fade-caption");

    var spanTextNode = document.createTextNode(album.name);
    domSpan.appendChild(spanTextNode);
    domSpan.appendChild(document.createElement("br"));
    domSpan.appendChild(document.createElement("br"));

    var artistText = "";
    var first = true;
    for(j=0; j<album.artists.length; j++) {
         var aArtistRef = document.createElement("a");
         aArtistRef.setAttribute("href","#/artist/" + album.artists[j].id + "/album");
        var spanTextArtist = document.createTextNode(album.artists[j].name);
        aArtistRef.appendChild(spanTextArtist);
        domSpan.appendChild(aArtistRef);
        domSpan.appendChild(document.createElement("br"));
    }

    domSpan.appendChild(document.createElement("br"));
    var spanTextYear = document.createTextNode(album.year);
    domSpan.appendChild(spanTextYear);
    domContainer.appendChild(domSpan);
    return domContainer;
}
function drawPage(element,size,  index, limit) {
    jQuery.ajax({url: "/api/albums?index=" + index + "&limit="+limit, async: true, success: function( data ) {
        var resultCount = data.albums.length;
        for( i=0; i<resultCount; i++ ) {
            var domContainer = document.createElement("div");
            domContainer.setAttribute("style", "width:" + size + "px; height:" + size + "px;");
            domContainer.setAttribute("class","box");

            var domImg = document.createElement("img");
            domImg.setAttribute("src", '/album/' + data.albums[i].id + '/cover.jpg');
            domImg.setAttribute("style","width:" + size + "px; height:" + size + "px;");

            domContainer.appendChild(domImg);

            element.append(domContainer);
        }
    }});
}

var squawkControllers = angular.module('squawkControllers', [])
.directive('pager', ["$window", "$rootScope", function ($window, $rootScope) {
  return {
    restrict: 'E',
      scope: {
            page: '=',
            pages: '='
      },
  link: function(scope, element, attrs) {
      console.log("pager:" + scope.page + ", pageCount:" + scope.pages);
      element.html('');
  }
    }
}])
.directive('albums', ["$window", "$rootScope", function ($window, $rootScope) {
  return {
    restrict: 'E',
      scope: {
            filter: '=filter',
            orderProp: '=orderProp',
            size: '=size',
            count: '=',
            page: '=',
            pages: '='
      },
   link: function(scope, element, attrs) {
      console.log("count:" + scope.count + ", pageSize:" + scope.page + ", pageCount:" + scope.pages);
      element.html('');
      drawPage(element, scope.size, 0, scope.page);

    scope.$watch('filter', function(newValue, oldValue) {
        console.log("new filter:"+ newValue);
    if(newValue != oldValue) {
    }
    });

/*      old_start = -1; old_end = -1;
      frag.setAttribute("id","album-list");
      load_rows(frag, row_ciel($window, scope.size), row_floor($window, scope.size), scope.size);
      element.append(frag);*/
  }
    }
}])
.directive('artist', ["$window", "$rootScope", function ($window, $rootScope) {
  return {
    restrict: 'E',
      scope: {
          letter: '=artist_filter'
      },
  link: function(scope, element, attrs) {
      element.html('');
      var frag = document.createDocumentFragment();
      $.get( "/api/artists", function( data ) {
          var old_letter = '';
          for(i=0; i<data.length; i++) {
              if( data[i].letter != old_letter ) {
                  old_letter = data[i].letter;
                  var letter = document.createElement("span");
                  letter.setAttribute("class","letter");
                  var spanTextLetter = document.createTextNode(data[i].letter);
                  letter.appendChild(spanTextLetter);
                  frag.appendChild(letter);
                  frag.appendChild(document.createElement("br"));
              }
              var aArtistRef = document.createElement("a");
              aArtistRef.setAttribute("href","#/artist/" + data[i].id + "/album");
              var spanTextArtist = document.createTextNode(data[i].name);
              aArtistRef.appendChild(spanTextArtist);
              frag.appendChild(aArtistRef);
              frag.appendChild(document.createElement("br"));
          }
          element.append(frag);
      });
      }
  };
}]);


    
squawkControllers.controller('AlbumListCtrl', ['$scope', '$http', '$window', function AlbumListCtrl($scope, $http, $window ) {
  $scope.orderProp = 'album';
  $scope.imageSize = getImageSize($window);
  $scope.pageSize = Math.floor($window.innerHeight/$scope.imageSize)*items_per_line;

  jQuery.ajax({url: "/api/albums?attributes=count", async: false, success: function( data ) {
    $scope.albumCount = data.count;
    $scope.pageCount = Math.ceil(data.count/$scope.pageSize);
  }});

/*    $scope.$watch('orderProp', function(newValue, oldValue) {
      if(newValue != oldValue) {
          scope.order = newValue;
      }
    });
  $window.onscroll = function() {
      load_rows(frag, row_ciel($window,getImageSize($window)), row_floor($window, getImageSize($window)), getImageSize($window));
  } */
}]);
    
squawkControllers.controller('ArtistListCtrl', ['$scope', '$http', function ArtistListCtrl($scope, $http) {
/*  $http.get('/api/artist').success(function(data) {
    $scope.artists = data;
  });
  $scope.orderProp = 'artist'; */
}]);

squawkControllers.controller('AlbumDetailCtrl', ['$scope', '$http', '$routeParams',  function AlbumDetailCtrl($scope, $http, $routeParams) {
  $http.get('/api/album/' + $routeParams.albumId).success(function(data) {
    $scope.album = data;
  });
  $scope.songsOrderProp = 'track';
  $scope.showGallery = 'true';
}]);
squawkControllers.controller('AlbumByArtistCtrl', ['$scope', '$http', '$routeParams', '$window',  function AlbumByArtistCtrl($scope, $http, $routeParams, $window) {
    alert( "get albums by artist: " + $routeParams.artistId );
    $http.get('/api/albums?artist-id=' + $routeParams.artistId + "&index=0&limit=10").success(function(data) {
    $scope.albums = data.albums;
  });
  $scope.songsOrderProp = 'album';
  $scope.imageSize = getImageSize($window);
  $window.onresize=function(){
    $scope.imageSize = getImageSize($window);
    $( ".list-cover" ).each(function( index ) {
      $( this ).width($scope.imageSize);
      $( this ).height($scope.imageSize);
    });
  };
}]);
