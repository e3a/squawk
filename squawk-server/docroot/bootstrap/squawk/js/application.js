var squawkApp = angular.module('squawkApp', [
'ngRoute',
'ngSanitize',
'ui.bootstrap',
'ngAside',
'angularUtils.directives.dirPagination',
"com.2fdevs.videogular",
'jkuri.gallery'
])
.controller('MainCtrl', function($scope, $aside) {
    var asideInstance = $scope.openAside = function openAside(position) {
      if( position == "right" ) {
          url = 'templates/search-menu.html';
      } else {
          url = 'templates/aside-menu.html';
      }

      $aside.open({
        placement: position,
        templateUrl: url,
        size: 'lg'
      });
    };
});

squawkApp.config(['$routeProvider',
function($routeProvider) {
$routeProvider.
when('/album', {
templateUrl: 'templates/album-list.html',
controller: 'AlbumListCtrl'
}).
when('/artist', {
templateUrl: 'templates/artist-list.html',
controller: 'ArtistListCtrl'
}).
when('/album/:albumId', {
templateUrl: 'templates/album-item.html',
controller: 'AlbumDetailCtrl'
}).
when('/artist/:artistId/album', {
templateUrl: 'templates/album-list.html',
controller: 'AlbumByArtistCtrl'
}).
when('/video', {
templateUrl: 'templates/video-list.html',
controller: 'VideoListCtrl'
}).
when('/video/:videoId', {
templateUrl: 'templates/video-item.html',
controller: 'VideoItemCtrl'
}).
when('/admin', {
templateUrl: 'templates/admin.html',
controller: 'AdminCtrl'
}).
otherwise({
redirectTo: '/album'
});
}]);
