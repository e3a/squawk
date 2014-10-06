var squawkApi = angular.module('squawkApi', [
'ngRoute',
'squawkControllers'
]);

squawkApi.config(['$routeProvider',
function($routeProvider) {
  $routeProvider.
  when('/main', {
  templateUrl: '/console/main.html',
  controller: 'ApiCtrl'
  }).
  when('/albums', {
  templateUrl: '/console/album-list.html',
  controller: 'AlbumsListCtrl'
  }).
  when('/album/:albumId', {
  templateUrl: '/console/album-detail.html',
  controller: 'AlbumDetailCtrl'
  }).
  when('/artists', {
  templateUrl: '/console/artist-list.html',
  controller: 'ArtistListCtrl'
  }).
  when('/artist/albums', {
  templateUrl: '/console/albums-by-artist-list.html',
  controller: 'AlbumByArtistListCtrl'
  }).
    when('/devices', {
    templateUrl: '/console/devices-list.html',
    controller: 'DevicesListCtrl'
    }).
    when('/statistics', {
    templateUrl: '/console/statistics.html',
    controller: 'StatisticsCtrl'
    }).
  otherwise({
  redirectTo: '/main'
  })
}]);
