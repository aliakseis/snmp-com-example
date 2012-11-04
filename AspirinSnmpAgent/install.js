
var objArgs = WScript.Arguments;

var agents_path = "HKLM\\SYSTEM\\CurrentControlSet\\Services\\SNMP\\Parameters\\ExtensionAgents\\";

var reg_path = "SOFTWARE\\Incentive Aspirin\\SNMPAgent\\CurrentVersion";

var agent_folder = objArgs(0)

var full_agent_path = agent_folder + "\\AspirinSnmpAgent.dll";


var WshShell = WScript.CreateObject("WScript.Shell");	

var i = 0;
var alreadyExist = false;
while(true)
{	
	try
	{
	    var bKey = WshShell.RegRead(agents_path + i);
	    if (bKey == reg_path)
	    {
	        alreadyExist = true;
	    }
	}
	catch(e)     	
	{
		break;
	}
	++i;
}
if (!alreadyExist)
{
    WScript.Echo("The following registry entry will be added: " + agents_path + i );
    WshShell.RegWrite(agents_path + i, reg_path, "REG_SZ");	
}

WshShell.RegWrite("HKLM\\" + reg_path + "\\Pathname", full_agent_path, "REG_SZ");

WScript.Echo("Success!");
