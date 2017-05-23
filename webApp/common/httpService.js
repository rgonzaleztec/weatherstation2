/*
  weatherStation TEC 2016
  Marcos Rodríguez Ovares
  Http Service
*/
'use strict';

angular.module('weatherStationApp')
.service('httpService',function($http){
  var th = this;

  th.getMeasures = function (id, limit,callback){
    $http({method: 'GET', url: '/getWeatherData/' + id + '/' + limit}).
              then(
                      function (response) {
                          callback(response);
                      },
                      function (response) {
                        callback(response);
                      }
              );
  };

});
