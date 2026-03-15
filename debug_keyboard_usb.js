importPackage(Packages.com.ti.debug.engine.scripting);
importPackage(Packages.com.ti.ccstudio.scripting.environment);
importPackage(Packages.java.lang);

var script = ScriptingEnvironment.instance();
var debugServer = null;
var debugSession = null;

function hex32(value)
{
    var masked;

    masked = value & 0xFFFFFFFF;
    return "0x" + java.lang.Long.toHexString(masked);
}

function logRegs(label)
{
    var pc;
    var lr;
    var sp;

    pc = debugSession.expression.evaluate("PC");
    lr = debugSession.expression.evaluate("LR");
    sp = debugSession.expression.evaluate("SP");

    script.traceWrite(label +
                      " PC=" + hex32(pc) +
                      " LR=" + hex32(lr) +
                      " SP=" + hex32(sp));
}

try
{
    script.traceBegin("c:/Anderson/Projetos/CCS_WorkSpace/freertos/debug_keyboard_usb.xml",
                      "DefaultStylesheet.xsl");
    script.traceSetConsoleLevel(TraceLevel.INFO);
    script.traceSetFileLevel(TraceLevel.ALL);
    script.setScriptTimeout(120000);

    debugServer = script.getServer("DebugServer.1");
    debugServer.setConfig("c:/Anderson/Projetos/CCS_WorkSpace/freertos/target_config.ccxml");
    debugSession = debugServer.openSession(".*");

    debugSession.target.connect();
    debugSession.memory.loadProgram("c:/Anderson/Projetos/CCS_WorkSpace/freertos/Debug/freertos.out");

    debugSession.breakpoint.removeAll();
    debugSession.breakpoint.add("app.c", 146);
    debugSession.breakpoint.add("startup_ccs.c", 260);
    debugSession.breakpoint.add("startup_ccs.c", 280);

    debugSession.target.restart();

    script.traceWrite("Target running. Pressione uma tecla no teclado USB agora.");
    debugSession.target.run();
    logRegs("Primeira parada:");

    debugSession.breakpoint.removeAll();
    debugSession.breakpoint.add("startup_ccs.c", 260);
    debugSession.breakpoint.add("startup_ccs.c", 280);

    script.traceWrite("Continuando a execucao para observar fault/default handler.");
    debugSession.target.run();
    logRegs("Segunda parada:");
}
catch (ex)
{
    script.traceWrite("EXCEPTION: " + ex);
}
finally
{
    if(debugSession != null)
    {
        try
        {
            debugSession.target.halt();
        }
        catch (haltEx)
        {
        }

        try
        {
            debugSession.target.disconnect();
        }
        catch (disconnectEx)
        {
        }

        try
        {
            debugSession.terminate();
        }
        catch (terminateEx)
        {
        }
    }

    if(debugServer != null)
    {
        try
        {
            debugServer.stop();
        }
        catch (stopEx)
        {
        }
    }

    script.traceEnd();
}
