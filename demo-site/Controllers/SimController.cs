using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Web;
using System.Web.Http;

namespace demo_site.Controllers
{
  public class SimController : ApiController
  {
    [DllImport("sentence-similarity.dll", EntryPoint = "compute_similarity", CallingConvention = CallingConvention.Cdecl,
      CharSet = CharSet.Ansi)]
    public static extern float compute_similarity(IntPtr pWne, string s1, string s2,
      [Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_BSTR)] out string[] vertices,
      [Out, MarshalAs(UnmanagedType.SafeArray, SafeArraySubType = VarEnum.VT_BSTR)] out string[] edges);

    [DllImport("sentence-similarity.dll", EntryPoint = "compute_similarity2", CallingConvention = CallingConvention.Cdecl)]
    public static extern float compute_similarity2(string s1, string s2);

    [DllImport(@"sentence-similarity.dll", EntryPoint = "connect_wne", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr connect_wne(string wn_path, string freq_path);

    [DllImport(@"sentence-similarity.dll", EntryPoint = "disconnect_wne", CallingConvention = CallingConvention.Cdecl)]
    public static extern void disconnect_wne(IntPtr pWne);

    // GET api/values
    public string GetSim(string s1, string s2)
    {
      IntPtr pWne = connect_wne(HttpContext.Current.Server.MapPath(@"~\dicts"),
        HttpContext.Current.Server.MapPath(@"~\dicts\freq.txt"));

      string[] vertices, edges;
      float sim = compute_similarity(pWne, s1, s2, out vertices, out edges);
      //float sim = compute_similarity2(s1, s2);
      
      return sim.ToString();
    }
    
    // GET api/values/5
    public string Get(int id)
    {
      return "value";
    }

    // POST api/values
    public void Post([FromBody]string value)
    {
    }

    // PUT api/values/5
    public void Put(int id, [FromBody]string value)
    {
    }

    // DELETE api/values/5
    public void Delete(int id)
    {
    }
  }
}