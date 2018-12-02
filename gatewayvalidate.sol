pragma solidity ^0.4.0;



contract gatewayauthorize{

	address public owner;

	address public subject;

	address public object;


	event ReturnAccessResult( 

		address indexed _from, 

		string _errmsg, 

		bool _result);



    bytes32[] deviceIdList;

	/*convert strings to byte32*/

	function stringToBytes32(string _str) public constant returns (bytes32){

		bytes memory tempBytes = bytes(_str);

		bytes32 convertedBytes;

		if(0 == tempBytes.length){

			return 0x0;

		}

		assembly {

			convertedBytes := mload(add(_str, 32))

		}

		return convertedBytes;

	}

	

	function gatewayauthorize() public{

		owner = msg.sender;

		object = msg.sender;

	}






	function AddId(string _id) public{


		bytes32 id = stringToBytes32(_id);

		if(msg.sender == owner){
            deviceIdList.push(id);
		}

		else throw;

	}



	function getIdIndex(string _id) public constant returns (uint _index){

		bytes32 id = stringToBytes32(_id);
		
		for(uint i=0; i < deviceIdList.length; i++){
		    if(deviceIdList[i] == id)
		        return i;
		}
        throw;
	}


	function deleteId(string _id) public{

        bytes32 id = stringToBytes32(_id);
		if(msg.sender == owner){
            for(uint i=0; i < deviceIdList.length; i++){
		        if(deviceIdList[i] == id)
		            delete deviceIdList[i];
		    }
		}

		else throw;

	}


	function deleteContract() public{

		if(msg.sender == owner){

			selfdestruct(this);

	    }

	}
	
	
	function validateId(string _id) public{
	    
	    bytes32 id = stringToBytes32(_id);
	    
        for(uint i=0; i < deviceIdList.length; i++){
	        if(deviceIdList[i] == id)
	             ReturnAccessResult(msg.sender, "Access authorized!", true);
		}
        ReturnAccessResult(msg.sender, "Invalid ID.... No access", false);
           
	}

}