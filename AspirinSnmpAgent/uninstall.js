var agents_path = "HKLM\\SYSTEM\\CurrentControlSet\\Services\\SNMP\\Parameters\\ExtensionAgents\\";
var reg_path = "SOFTWARE\\Incentive Aspirin\\SNMPAgent\\CurrentVersion";



var WshShell = WScript.CreateObject("WScript.Shell");	

WScript.Echo("Deleting registry " + agents_path + " entry");
var i = 0;    
while(true)
{	
    try
	{
	    var bKey = WshShell.RegRead(agents_path + i);
	    if (bKey == reg_path)
	    {
	        WshShell.RegDelete(agents_path + i);	        
	        WScript.Echo("Completed!");
	        break;
	    }	        
    }
	catch(e)     	
	{
	    WScript.Echo("***Failed. Unable to delete entry: " + e.message);
	    break;
    }
	++i;
}

WScript.Echo("Deleting registry HKLM\\SOFTWARE\\Incentive Aspirin\\SNMPAgent\\ entry");
try
{
    WshShell.RegDelete("HKLM\\SOFTWARE\\Incentive Aspirin\\SNMPAgent\\CurrentVersion\\");
    WshShell.RegDelete("HKLM\\SOFTWARE\\Incentive Aspirin\\SNMPAgent\\");
    WScript.Echo("Completed!");
}
catch(e)     	
{
    WScript.Echo("***Failed. Unable to delete registry entries: " + e.message);	
}
