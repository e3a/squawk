var squawkControllers = angular.module('squawkControllers', []);

squawkControllers.controller('ApiCtrl', ['$scope', '$http', function AlbumListCtrl($scope, $http) {
  /* $http.get('/api/albums').success(function(data) {
    $scope.albums = data.albums;
  });*/
}]);
  
squawkControllers.controller('StatisticsCtrl', ['$scope', '$http', function StatisticsCtrl($scope, $http) {
  $http.get('/api/statistics').success(function(data) {
    squawkApi.editor.set(data);
  });
  $scope.$on('$viewContentLoaded', function() {
    var container = document.getElementById('jsoneditor');
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor = new jsoneditor.JSONEditor(container, options);
  });
}]);
squawkControllers.controller('AlbumsListCtrl', ['$scope', '$http', function AlbumListCtrl($scope, $http) {
  $http.get('/api/album').success(function(data) {
    squawkApi.editor.set(data);
  });
  $scope.$on('$viewContentLoaded', function() {
    var container = document.getElementById('jsoneditor');
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor = new jsoneditor.JSONEditor(container, options);
  });
}]);
squawkControllers.controller('ArtistListCtrl', ['$scope', '$http', function ArtistListCtrl($scope, $http) {
  $http.get('/api/artist').success(function(data) {
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor.set(data, options);
  });
  $scope.$on('$viewContentLoaded', function() {
    var container = document.getElementById('jsoneditor');
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor = new jsoneditor.JSONEditor(container, options);
  });
}]);
squawkControllers.controller('AlbumDetailCtrl', ['$scope', '$http', '$routeParams',  function AlbumDetailCtrl($scope, $http, $routeParams) {
  $http.get('/api/album/' + $scope.id ).success(function(data) {
    squawkApi.editor.set(data);
  });
  $scope.$on('$viewContentLoaded', function() {
    var container = document.getElementById('jsoneditor');
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor = new jsoneditor.JSONEditor(container, options);
  });
  $scope.$watch('id', function(newValue, oldValue) {
    if(newValue != oldValue) {
      $http.get('/api/album/' + $scope.id ).success(function(data) {
	squawkApi.editor.set(data);
      });
    }
  });
  $scope.id = 1;
}]);
squawkControllers.controller('AlbumByArtistListCtrl', ['$scope', '$http', '$routeParams',  function AlbumByArtistListCtrl($scope, $http, $routeParams) {
  $http.get('/api/artist/' + $scope.artist_id + '/album' ).success(function(data) {
    squawkApi.editor.set(data);
  });
  $scope.$on('$viewContentLoaded', function() {
    var container = document.getElementById('jsoneditor');
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor = new jsoneditor.JSONEditor(container, options);
  });
  $scope.$watch('artist_id', function(newValue, oldValue) {
    if(newValue != oldValue) {
      $http.get('/api/artist/' + $scope.artist_id + '/album' ).success(function(data) {
	squawkApi.editor.set(data);
      });
    }
  });
  $scope.artist_id = 1;
}]);
squawkControllers.controller('DevicesListCtrl', ['$scope', '$http', function DevicesListCtrl($scope, $http) {
  $http.get('/api/devices').success(function(data) {
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor.set(data, options);
  });
  $scope.$on('$viewContentLoaded', function() {
    var container = document.getElementById('jsoneditor');
    var options = {
      modes: ['code', 'view'],
      name: 'Squawk API Console',
      search: true
    };
    squawkApi.editor = new jsoneditor.JSONEditor(container, options);
  });
}]);
