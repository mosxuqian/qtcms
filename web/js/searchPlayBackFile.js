	var bool = 1, //本地远程回放控制  0为远程 1为本地
		recTotal = 0,  //文件检索总数。
		nowDevID=null, //当前选中设备ID
		searchDate,//进行搜索的日期
		searchType,//显示在表格上的文件的类型
		searchSTOP=1;  //搜索停止. 包括搜索结束,搜索过程中失败
	//搜索远程录像
	function setDevData2ocx(){

		var b = 1;
		if(bool){  //本地
			if(oPlaybackLocl.SetSynGroupNum(4)){ 
				//alert(lang.Failed_to_set_the_number_of_synchronization_group);
				b = 0
			}
		}else{  //远程
			var oDevData=$('#dev_'+nowDevID).data('data'),
			oChannel =oCommonLibrary.GetChannelList(oDevData.dev_id);
			
			/*console.log('-------------参数填充到控件------------------');
			console.log(nowDevID)
			console.log('当前设备');
			console.log(oDevData);
			console.log('当前设备所属通道');
			console.log(oChannel);
			console.log('-------------参数填充到控件------------------')*/

			if(oPlayBack.setDeviceHostInfo(oDevData.address,oDevData.port,oDevData.eseeid)){
				//alert(lang.Failed_to_set_the_IP_address_or_port_is_not_legal);
				b = 0;
			}

			if(oPlayBack.setDeviceVendor(oDevData.vendor)){
				//alert(lang.Failed_to_set_vendor_is_empty);
				b = 0;
			}
			oPlayBack.setUserVerifyInfo(oDevData.username,oDevData.password);
			var intoWindsChl = $("#channelvideo").find('input:checkbox');
			if(intoWindsChl.length != 0){
				var i= 0;
				/*console.log('选中的通道');
				console.log(intoWindsChl);*/
				intoWindsChl.each(function(index){
					//console.log('当前checkbox状态:'+$(this).is(':checked'));
					if($(this).is(':checked') && oChannel[index]){
						//console.log(i+'//'+oChannel[index]);
						if(oPlayBack.AddChannelIntoPlayGroup(i,oChannel[index])){
							b = 0;
						};
						i++;
					}
				});
			}else{
				for(var i=0;i<oDevData.channel_count;i++){
					if(oPlayBack.AddChannelIntoPlayGroup(i,oChannel[i])){
						b = 0;
					}
				}
			}
		}
		return b;
	}
  	function berorSerchShowHint() {
		
			$('#fileRec').stop(true,true).show().find('p').show()
			         .end().find('span').width(0)
					 .end().find('h5').html('0%')
					 .end().find('h4').html(lang.Retrieving)
  	}
	var typeHint = [];
		typeHint[1] = lang.Timing;
		typeHint[2] = lang.Mobile;
		typeHint[4] = lang.Warning;
		typeHint[8] = lang.Manual;
		typeHint[15] = lang.All;
    var color = [];
	  color[1] = '#F00';
	  color[2] = '#33CC00';
	  color[4] = '#FFFF00';
	  color[8] = '#0000FF';
	function ocxsearchVideo(){

		recTotal = 0;
        bool &&  berorSerchShowHint();
		//getAudioObj().GroupStop();
         
		 var bo = document.getElementById('commonLibrary').getLanguage() =='en_PR',
		  
            isJalaali = document.getElementById('commonLibrary').getIsPersian(),
			
		    jaChecked = bo && isJalaali;

		var type = $('#type input[data]').attr('data'),
		
			date = $("div.calendar span.nowDate").html(),

			oList = $('div.dev_list'),

			devData = bool ?  oList.find('span.device:eq('+localSearchWindNum+')').data('data') : (oList.find('li.sel span.device').data('data') || oList.find('span.channel.sel').parent('li').parent('ul').prev('span.device').data('data'));

            date = jaChecked ? jalaali_to_solar(date) : date;

			searchDate =  date ;
			
			searchType = type;
			
		//var devData = $('div.dev_list li.sel span.device').data('data') || $('div.dev_list span.channel.sel').parent('li').parent('ul').prev('span.device').data('data');
			//console.log($('div.dev_list li.sel span.device'));
		if(!devData){
			$('div.dev_list span.device,li').removeClass('sel');
			devData = $('div.dev_list span.device:first').parent('li').addClass('sel')
						.end().data('data');
		}else{
			$('div.dev_list li,span').removeClass('sel');
			$('#dev_'+devData.dev_id).parent('li').addClass('sel');
		}

		nowDevID = bool ? '':devData.dev_id;

		setDevData2ocx();
        
		
		if(bool){  // 本地
		  localSearchWindNum=0;
         
		  searchLocalFile(localSearchWindNum,date,type);
		 // console.log(localSearchWindNum+' '+date+' '+type);

			/*var chl ='';
			for (var i=1;i<=devData.channel_count;i++){
				chl+=i+';';
			};
			if(oPlaybackLocl.searchDateByDeviceName(devData.name)){
				alert(T('devData.name',devData.name));
				return;
			}
			oPlaybackLocl.searchVideoFile(devData.name,date,startTime,endTime,chl);*/
		}else{
			// 远程
			var startTime =gettime($('div.timeInput:eq(0) input')) || '00:00:00';
			var endTime =gettime($('div.timeInput:eq(1) input')) || '23:59:59';
			
			var chl = 0;

			for (var i=0;i<devData.channel_count;i++){
				chl += 1 << i;
			};
			var sta = oPlayBack.startSearchRecFile(chl,type,date+' '+startTime,date+' '+endTime)
			/*console.log(chl+'+'+type+'+'+startTime+'+'+endTime+'搜索文件的状态:'+sta);
			if(sta != 0){
				//alert(T('Failed_to_retrieve_video',devData.name,typeHint[type]));
			}*/
		}
	}

	function searchLocalFile(wind,date,type){
		/*if(wind > 64){
			searchSTOP=1;
			return;
		}*/

		var bo = document.getElementById('commonLibrary').getLanguage() =='en_PR',
		  
            isJalaali = document.getElementById('commonLibrary').getIsPersian(),
			
		    jaChecked = bo && isJalaali;
		 
		var type = type || $('#type input[data]').attr('data');
		
		var date = date || (jaChecked ? jalaali_to_solar($("div.calendar span.nowDate").html()) : $("div.calendar span.nowDate").html());

		
        searchDate =  date ;
		
		searchType = type
		//var oDevList = $('div.dev_list span.device');
		/*if(key >  (oDevList.length-1))
			return;
		//console.log(localSearchDevNum);*/
		

		//var name = oDevList.eq(key).data('data').name;

		/*console.log('搜索当前设备:'+name+'参数日期为:'+date+'参数文件类型为:'+type+'----------搜索状态为:'+oPlaybackLocl.searchVideoFileEx(name,date,type));*/
		//console.log('当前本地搜索窗口号:'+wind+'//日期:'+date+'//开始时间00:00:00//23:59:59//搜索文件类型:'+type);
		oPlaybackLocl.searchVideoFileEx2(wind,date,'00:00:00','23:59:59',type);
		//console.log('调用是否成功(0-yes,1-no)： '+oPlaybackLocl.searchVideoFileEx2(wind,date,'00:00:00','23:59:59',type));
	}
	function showRecProgress(now){  //回访检索文件进度
		now = now || 0;

		if(now != 0 && now>=recTotal){
			searchSTOP=1;
			now=recTotal;
		}

		//console.log(now+'---------------'+recTotal);

		var con = lang.Retrieving,
			p =now/recTotal*100,
			str = (now/recTotal*100).toString().slice(0,5);
			str = str == 'NaN'?'':str+'%';

		if(recTotal == now){
			con = lang.Retrieval_completed;
		}

		$('#fileRec').stop(true,true).find('span').show().width(p-2)
		             .end().find('h5').html(str)
		             .end().find('h4').html(con);
		return str; 
	}
     function showLimitTips(){
		 $('#limitTips p').html(_T('no_limit')).css('color','red');
		 $('#limitTips').show();
		 var timer = setTimeout(function(){
			  $('#limitTips').fadeOut(1500);
			  clearTimeout(timer);
			 },1500);
		 
	 }
	function recFileSearchFailCallback(data){
		/*console.log('recFileSearchFailCallback');
		console.log(data);
*/
		/*
		远程文件过多后  导致请求次数变多.

		请求过程中仍和一次请求都有可能失败.

		后续所有文件信息接收到后 开始画到UI上

		已经搜索到的文件如果显示达到前台的话。 那么没有搜索到的文件是否能继续播放.
		*/

		searchSTOP=1;

		var hint = [lang.Parameter_error,lang.Connection_Failed,lang.not_complete];
		$('#fileRec h4').html('<h4 style="color:red;">'+hint[(parseInt(data.parm)-1)]+'</h4>')
						.end().show();
		setTimeout(function(){
			$('#fileRec').stop(true,true).fadeOut(1500);
		},1000);
		//console.log(recFile)
		//recFile.length !=0 && RecRemoteFile2UI(recFile);
	}

	function RecfinishCallback(data){ //检索完成回调
		/*console.log('------------RecfinishCallback文件总数-----------'+data.total);*/
       //alert('------------RecfinishCallback文件总数-----------'+data.total);
		 
		recTotal = data.total || 0;	

		if(recTotal==0)
			searchSTOP=1;

		showRecProgress();
		
	}

	function initrecFileOcx(obj){
		if(!$('#dev_'+nowDevID)[0]){
			//obj.remove();
			$('#fileRec').stop(true,true).hide();
			if($('ul.filetree:eq(0) span.device:eq(0)')[0]){
				nowDevID = $('ul.filetree:eq(0) span.device:eq(0)').parent('li').addClass('sel').end().data('data').dev_id;
			}
		}else{
			nowDevID = $('#dev_'+nowDevID).parent('li').addClass('sel').end().data('data').dev_id;
		}
		//console.log('初始化后的设备ID:'+nowDevID);
	}

	/*排序方法	*/
	function TimeAsc(a,b){
		return time2Sec(a.start.split(' ')[1]) - time2Sec(b.start.split(' ')[1]);
	}

	function TimeDes(a,b){
		return time2Sec(b.start.split(' ')[1]) - time2Sec(a.start.split(' ')[1]);
	}

	function ChlAsc(a,b){
		return a.channelnum - b.channelnum;
	}

	function ChlDes(a,b){
		return b.channelnum - a.channelnum;
	}

	function TypeAsc(a,b){
		return a.types - b.types;
	}

	function TypeDes(a,b){
		return b.types - a.types;
	}				
	/*排序方法	*/	
     function searchStartcallback(){
		 searchSTOP = 0;
	   berorSerchShowHint();	 
	}
	$(function(){
		oPlayBack.AddEventProc('RecFileInfo','RecFileInfoCallback(data)');  //搜索远程回放回调
		oPlayBack.AddEventProc('recFileSearchFail','recFileSearchFailCallback(data)'); //搜索远程回放失败回调
		oPlayBack.AddEventProc('recFileSearchFinished','RecfinishCallback(data)');  //搜索远程回放完成回调
		oPlayBack.AddEventProc('recFileSearchStart','searchStartcallback()');
		
		oPlaybackLocl.AddEventProc('GetRecordFileEx','RecFileInfoCallback(data)'); //本地回访回调
		oPlaybackLocl.AddEventProc('SearchRecordOver','SearchRecordOverCallback(data)');
		oPlaybackLocl.AddEventProc('ThrowException','ThrowExceptionCallback(data)');
	})