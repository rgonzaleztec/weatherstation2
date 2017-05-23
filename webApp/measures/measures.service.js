/*
  weatherStation TEC 2016
  Marcos Rodríguez Ovares
  Measures Service
*/
'use strict';

angular.module('weatherStationApp')
.service('MeasuresService',function(httpService){
  var th = this;

  th.getMeasures = function(id, limit, callback){
        httpService.getMeasures(id, limit, function(response){
          callback(response);
        });
  };

});
