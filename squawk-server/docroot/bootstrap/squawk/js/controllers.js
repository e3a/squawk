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
        $http.get("/api/album?attributes=id,name,artist&index=" + start + "&limit=30").success( function( response ) {
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
    $scope.video = data;
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
