using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Net.Http;
using System.Runtime.InteropServices;
using System.Web.Http;

namespace demo_site.Controllers
{
  public class SimController : ApiController
  {
    [DllImport("sentence-similarity.dll", EntryPoint = "compute_similarity", CallingConvention = CallingConvention.Cdecl)]
    public static extern float compute_similarity(IntPtr pWne, string s1, string s2);

    [DllImport("sentence-similarity.dll", EntryPoint = "compute_similarity2", CallingConvention = CallingConvention.Cdecl)]
    public static extern float compute_similarity2(string s1, string s2);

    [DllImport(@"sentence-similarity.dll", EntryPoint = "connect", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr connect(string wn_path, string freq_path);

    [DllImport(@"sentence-similarity.dll", EntryPoint = "disconnect", CallingConvention = CallingConvention.Cdecl)]
    public static extern void disconnect(IntPtr pWne);

    // GET api/values
    public string GetSim(string s1, string s2)
    {
      IntPtr pWne = connect("C:\\Users\\XinL\\Downloads\\libs\\WordNet-3.0\\dict", 
        "C:\\Projects\\sentence-similarity\\dicts\\freq.txt");
      float sim = compute_similarity(pWne, s1, s2);
      //float sim = compute_similarity2("i like that bachelor", "i like that unmarrided man");

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