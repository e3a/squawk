var squawkApp = angular.module('squawkApp', [
'ngRoute',
'squawkControllers'
]);
  
squawkApp.config(['$routeProvider',
function($routeProvider) {
$routeProvider.
when('/album', {
templateUrl: 'album-list.html',
controller: 'AlbumListCtrl'
}).
when('/artist', {
templateUrl: 'artist-list.html',
controller: 'ArtistListCtrl'
}).
when('/album/:albumId', {
templateUrl: 'album.html',
controller: 'AlbumDetailCtrl'
}).
when('/artist/:artistId/album', {
templateUrl: 'album-by-artist.html',
controller: 'AlbumByArtistCtrl'
}).
otherwise({
redirectTo: '/album'
});
}]);

    