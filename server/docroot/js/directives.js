var squawkControllers = angular.module('squawkControllers', [])
.directive('artist', ["$window", "$rootScope", function ($window, $rootScope) {
  return {
    restrict: 'E',
    controller: 'ArtistListCtrl',
    transclude: true,
      scope: {
          artists: '=artists'
      },
  template: function(scope, element, attrs) {
      element.html('');
      var frag = document.createDocumentFragment();
//      $.get( "/api/artists", function( data ) {
          var old_letter = '';
      alert(scope.artists);
          for(i=0; i<scope.artists.length; i++) {
              if( artists[i].letter != old_letter ) {
                  old_letter = scope.artists[i].letter;
                  var letter = document.createElement("span");
                  letter.setAttribute("class","letter");
                  var spanTextLetter = document.createTextNode(scope.artists[i].letter);
                  letter.appendChild(spanTextLetter);
                  frag.appendChild(letter);
                  frag.appendChild(document.createElement("br"));
              }
              var aArtistRef = document.createElement("a");
              aArtistRef.setAttribute("href","#/artist/" + scope.artists[i].id + "/album");
              var spanTextArtist = document.createTextNode(scope.artists[i].name);
              aArtistRef.appendChild(spanTextArtist);
              frag.appendChild(aArtistRef);
              frag.appendChild(document.createElement("br"));
          }
          element.append(frag);
//      });
      }
  };
}]);
