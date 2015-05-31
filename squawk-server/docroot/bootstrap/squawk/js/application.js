var squawkApp = angular.module('squawkApp', [
'ngRoute',
'ui.bootstrap',
'ngAside',
'angularUtils.directives.dirPagination',
])
.controller('MainCtrl', function($scope, $aside) {

  $scope.asideState = {
    open: false
  };

  $scope.openAside = function(position, backdrop) {
    $scope.asideState = {
      open: true,
      position: position
    };

    function postClose() {
      $scope.asideState.open = false;
    }

    $aside.open({
      templateUrl: 'templates/aside-menu.html',
      placement: position,
      size: 'sm',
      backdrop: backdrop,
      controller: function($scope, $modalInstance) {
        $scope.ok = function(e) {
          $modalInstance.close();
          e.stopPropagation();
        };
        $scope.cancel = function(e) {
          $modalInstance.dismiss();
          e.stopPropagation();
        };
      }
    }).result.then(postClose, postClose);
  }
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
templateUrl: 'templates/video-list.html',
controller: 'VideoListCtrl'
}).
otherwise({
redirectTo: '/album'
});
}]);
