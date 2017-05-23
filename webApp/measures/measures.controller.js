/*
  weatherStation TEC 2016
  Marcos Rodríguez Ovares
  Measures Controller
*/
'use strict';

angular.module('weatherStationApp')
.controller('MeasuresController',['$state', 'MeasuresService',function($state, MeasuresService){
  var th = this;

  th.weatherMeasures = [];
  th.ready = false;

  th.refresh = function(id, limit){
    setInterval(function(){
                      MeasuresService.getMeasures(id, limit,function (res) {
                        res = res.data;
                          if (res.err){
                              th.weatherMeasures = [];
                              console.log("HTTP ERROR");
                              th.ready = false;
                          }else{
                              th.weatherMeasures = res.data;
                              th.ready = true;
                          }
                          //console.log(res);
                      });
                    }, 5000)
  };

  th.refresh('developmentStation',10);

}]);
