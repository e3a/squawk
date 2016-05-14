squawkApp.controller('AlbumListCtrl', ['$scope', '$http', '$window', function AlbumListCtrl($scope, $http, $window ) {
    $scope.albums = [];
    $scope.total_albums = 0;
    $scope.pageSize = 30; //Math.floor($window.innerHeight/$scope.imageSize)*items_per_line;
    getResultsPage(1);

    $scope.pagination = {
        current: 1
    };

    $scope.pageChanged = function(newPage) {
        getResultsPage(newPage);
    };

    function getResultsPage(pageNumber) {
        console.log("get albums, pageNumber: " + pageNumber);
        var start = ( pageNumber - 1 ) * 30;
        $http.get("/api/album?attributes=id,title,artist&page=" + start + "&limit=30&sort=title,asc").success( function( response ) {
            $scope.albums_count = response.count;
            $scope.albums = response.albums;
    console.log("get albums: " + response.albums.length);
        });
    }


/*
  $scope.imageSize = 200; //getImageSize($window);
  $scope.currentPage = 1;
  $scope.pageSize = 30; //Math.floor($window.innerHeight/$scope.imageSize)*items_per_line;
    var start = pageNumber * 30;
    $http.get("/api/album?attributes=id,name,artist&index=" + start + "&limit=" + pageSize).success( function( response ) {
        $scope.albums_count = response.count;
        $scope.albums = response.albums;
console.log("get albums: " + response.albums.length);
    }); */
}]);
squawkApp.controller('ArtistListCtrl', ['$scope', '$http', function ArtistListCtrl($scope, $http) {
  $http.get('/api/artist').success(function(data) {
    $scope.artists = data;
  });
}]);
squawkApp.controller('VideoListCtrl', ['$scope', '$http', function VideoListCtrl($scope, $http) {
  $http.get('/api/video').success(function(data) {
    $scope.videos = data;
  });
}]);
squawkApp.controller('VideoItemCtrl', ['$scope', '$http', '$routeParams', '$sce', function VideoItemCtrl($scope, $http, $routeParams, $sce) {
    $http.get('/api/video/' + $routeParams.videoId).success(function(data) {
        $scope.video = data;
        $scope.video.id = $routeParams.videoId;
        $scope.config = {
            preload: "none",
            sources: [
                {src: $sce.trustAsResourceUrl("/video/" + $routeParams.videoId + "." + data.ext), type: data.mime_type},
            ],
            theme: {
                url: "http://www.videogular.com/styles/themes/default/latest/videogular.css"
            }
        };
    });
}]);
squawkApp.controller('AlbumByArtistCtrl', ['$scope', '$http', '$routeParams', '$window',  function AlbumByArtistCtrl($scope, $http, $routeParams, $window) {
    $scope.albums = [];
    $scope.total_albums = 0;
    $scope.pageSize = 30; //Math.floor($window.innerHeight/$scope.imageSize)*items_per_line;
    getResultsPage(1);

    $scope.pagination = {
        current: 1
    };

    $scope.pageChanged = function(newPage) {
        getResultsPage(newPage);
    };

    function getResultsPage(pageNumber) {
        console.log("get albums by artist, pageNumber: " + pageNumber);
        var start = ( pageNumber - 1 ) * 30;
        $http.get('/api/album?attributes=id,name,artist&artist=' + $routeParams.artistId + "&index=" + start + "&limit=30").success(function(data) {
            $scope.albums_count = data.count;
            $scope.albums = data.albums;
    console.log("get albums by artist: " + data.albums.length);
        });
    }


/*    $scope.currentPage = 1;
    $scope.pageSize = 30; //Math.floor($window.innerHeight/$scope.imageSize)*items_per_line;

    var start = pageNumber * 30;

    $http.get('/api/album?attributes=id,name,artist&artist=' + $routeParams.artistId + "&index=" + start + "&limit=" + pageSize).success(function(data) {
        $scope.albums_count = response.count;
        $scope.albums = data.albums;
console.log("get albums by artist: " + data.albums.length);
  }); */
}]);
squawkApp.controller('AlbumDetailCtrl', ['$scope', '$http', '$routeParams',  function AlbumDetailCtrl($scope, $http, $routeParams) {
    $http.get('/api/album/' + $routeParams.albumId).success(function(data) {
        $scope.album = data;
    });
    $scope.songsOrderProp = 'track';
    $scope.showGallery = 'true';
}]);
squawkApp.controller('AdminCtrl', ['$scope', '$http', function AdminCtrl($scope, $http) {
    $scope.panes = [
        { title:"Settings",     content:"templates/config.html" , active: true},
        { title:"Dorectories",  content:"templates/directories.html"},
        { title:"Statistic",    content:"templates/statistic.html"},
        { title:"Upnp",         content:"templates/upnpevents.html"},
        { title:"About",        content:"templates/about.html"}
    ];
    $http.get('/api/statistic').success(function(data) {
        $scope.statistic = data;
    });
    $http.get('/api/upnp/event').success(function(data) {
        $scope.events = data;
    });
    $http.get('/api/upnp/device').success(function(data) {
        $scope.devices = data;
    });
}]);
squawkApp.controller('MusicSearchCtrl', ['$scope', '$http', '$routeParams',  function MusicSearchCtrl($scope, $http, $routeParams) {
    $scope.musicSearch = {};
    $scope.musicSearch.albums = {};
    $scope.musicSearch.artists = {};

/*    $scope.update = function(musicSearch) {
        alert( "update:" + musicSearch );
      }; */
    $scope.$watch(function(scope) { return $scope.musicSearch.term },
        function(newValue, oldValue) {
            if( newValue && newValue.length > 3 ) {
                $http.get('/api/album?attributes=id,name,artist&index=0&limit=10&order=name&name=' + newValue).success(function(data) {
                    $scope.musicSearch.albums  = data.albums;
                });
                $http.get('/api/artist?name=' + newValue).success(function(data) {
                    $scope.musicSearch.artists  = data;
                });
            }
        }
    );
}]);
squawkApp.controller('BrowseCtrl', ['$scope', '$http', '$routeParams',  function BrowseCtrl($scope, $http, $routeParams) {
	$http.get('/api/browse/' +  $routeParams.objectid ).success(function(data) {
		$scope.objects_count = data.objects_count;
		$scope.objects = data.objects;
	});
}]);
