/*******************************************************************************
 * JGraph.js for HS Quick Look
 * 
 * Authors: Soki Sakurai, Hirokazu Odaka
 * Date: 2013-**-**
 * Date: 2014-12-20 | Hirokazu Odaka | new design.
 * 
 ******************************************************************************/

/* Global variable */
var HSQuickLook = HSQuickLook || {};

/* The anonymous function of this script */
(function() {
  /***************************************************************************
   * Exported variables
   */
  HSQuickLook.graph = {};

  /***************************************************************************
   * Object prototype TrendCurve
   */
  HSQuickLook.graph.TrendCurve = function() {
    var currentData = [void 0, void 0];

    this.placeholder = "";
    this.capacity = -1;
    this.differentialMode = false;
    this.upperBound = 0.0;
    
    this.data = {
      label: "data1",
      color: "black",
      points: { show: true, radius: 1, fillColor: "black" },
      data: []
    };

    this.options = {
      legend: { show: true, position: "nw" },
      series: { lines: { show: true, lineWidth: 1 }},
      xaxis: { min: -1.0, max: +1.0 },
      yaxis: { min: -1.0, max: +1.0 }
    };

    this.setCurrentData = function(x, y) {
      currentData[0] = x;
      currentData[1] = y;
    };

    this.getCurrentDataX = function() {
      return currentData[0];
    };

    this.getCurrentDataY = function() {
      return currentData[1];
    };
  };

  var TrendCurve = HSQuickLook.graph.TrendCurve;

  TrendCurve.prototype.getLastYValue = function() {
    var data = this.data.data,
        n = data.length;

    if (n>0) {
      return data[n-1][1];
    } else {
      return void 0;
    }
  };

  TrendCurve.prototype.setRangeX = function(range) {
    this.options.xaxis.min = range[0];
    this.options.xaxis.max = range[1];
  };

  TrendCurve.prototype.setRangeY = function(range) {
    this.options.yaxis.min = range[0];
    this.options.yaxis.max = range[1];
  };

  TrendCurve.prototype.setCapacity = function(capacity) {
    var data = this.data.data;
    if (capacity>0 && capacity<data.length) {
      data.splice(0, data.length-capacity);
    }
    this.capacity = capacity;
  };
  
  TrendCurve.prototype.plot = function(query) {
    $.plot($(this.queryToPlaceholder), [this.data], this.options);
  };
  
  TrendCurve.prototype.pushData = function(dataPoint) {
    var data = this.data.data,
        size = data.length,
        capacity = this.capacity,
        lastDataX = this.getCurrentDataX(),
        lastDataY = this.getCurrentDataY(),
        newDataX = dataPoint[0],
        newDataY = dataPoint[1];

    this.setCurrentData(newDataX, newDataY);
    
    if (this.differentialMode == true) {
      if (lastDataX === void 0) {
        return;
      }
      if (newDataX-lastDataX > 0.0) {
        if (newDataY < lastDataY) {
          newDataY += this.upperBound;
        }
        newDataY = (newDataY-lastDataY)/(newDataX-lastDataX);
        data.push([newDataX, newDataY]);
        if (capacity>0 && size>=capacity) {
          data.shift();
        }
      }
    } else {
      data.push([newDataX, newDataY]);
      if (capacity>0 && size>=capacity) {
        data.shift();
      }
    }
  };

  /***************************************************************************
   * Object prototype MultiTrendCurves
   */
  HSQuickLook.graph.MultiTrendCurves = function() {
    var trendCurves = [],
        data = [],
        counter = 0;
    
    this.placeholder = "";
    this.refreshCycle = 4;
    this.refreshPhase = 1;
    this.timeOrigin = void 0;
    this.xWidth = 600.0;
    this.yMin = -1.0;
    this.yMax = +1.0;
    this.options = {
      legend: { show: true, position: "nw" },
      series: { lines: { show: true, lineWidth: 1 }},
      xaxis: { axisLabel: "Time (s)", min: -1.0, max: +1.0 },
      yaxis: { axisLabel: "", min: -1.0, max: +1.0 }
    };

    this.addTrendCurve = function(sourceID, curve) {
      trendCurves[sourceID] = curve;
      data.push(curve.data);
    };

    this.getTrendCurve = function(sourceID) {
      return trendCurves[sourceID];
    };

    this.plot = function() {
      if (counter == this.refreshCycle) {
        counter = 0;
      }
      if (counter == this.refreshPhase) {
        $.plot($(this.placeholder), data, this.options);
      }
      counter += 1;
    };
  };

  var MultiTrendCurves = HSQuickLook.graph.MultiTrendCurves;

  MultiTrendCurves.prototype.setRangeX = function(range) {
    this.options.xaxis.min = range[0];
    this.options.xaxis.max = range[1];
  };

  MultiTrendCurves.prototype.setRangeY = function(range) {
    this.yMin = range[0];
    this.yMax = range[1];
    this.options.yaxis.min = range[0];
    this.options.yaxis.max = range[1];
  };

  MultiTrendCurves.prototype.resetRangeY = function() {
    this.options.yaxis.min = this.yMin;
    this.options.yaxis.max = this.yMax;
  };

  MultiTrendCurves.prototype.adjustRangeX = function(x) {
    this.options.xaxis.min = x-this.xWidth+0.5;
    this.options.xaxis.max = x+0.5;
  };
  
  MultiTrendCurves.prototype.adjustRangeY = function(y) {
    if (y === void 0) { return; }

    var y0 = this.options.yaxis.min,
        y1 = this.options.yaxis.max,
        w = y1 - y0,
        r = (y-y0)/w,
        s = 1.0,
        c = 0.95;

    if (r > c) {
      s = r/c;
      y1 = y0 + w*s;
      this.options.yaxis.max = y1;
    }
    else if (r < 1-c) {
      s = (1.0-r)/c;
      y0 = y1 - w*s;
      this.options.yaxis.min = y0;
    }
  };
})(); /* end of the anonymous function */
