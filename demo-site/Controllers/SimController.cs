using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Web;
using System.Web.Http;
using demo_site.Models;

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
    public DemoSimResult GetSim(string s1, string s2)
    {
      IntPtr pWne = connect_wne(HttpContext.Current.Server.MapPath(@"~\dicts"),
        HttpContext.Current.Server.MapPath(@"~\dicts\freq.txt"));

      string[] vertices, edges;
      float sim = compute_similarity(pWne, s1, s2, out vertices, out edges);
      //float sim = compute_similarity2(s1, s2);

      return new DemoSimResult()
             {
               Sim = sim,
               Graph = GetGraph(vertices, edges),
               Vertices = vertices,
               Edges = edges
             };
    }

    private GraphNode GetGraph(string[] vertices, string[] edges)
    {
      var root = new GraphNode(){Words = "root", Children = new List<GraphNode>()};
      var nodes = new Dictionary<string, GraphNode>();
      var ends = new List<GraphNode>();

      foreach (var edge in edges)
      {
        string[] pair = edge.Split('-');

        GraphNode source;
        // TODO here assume souce(pair[0]) doesn't exist in nodes, 
        //    skip if exist which doesn't quite right if one node have multiple parents
        if (!nodes.ContainsKey(pair[0]))
        {
          source = new GraphNode()
                   {
                     Index = pair[0],
                     Words = vertices[Convert.ToInt32(pair[0])]
                   };
          nodes.Add(pair[0], source);
        }
        else
          source = nodes[pair[0]];

        var removeEnd = ends.FirstOrDefault(e => e.Index == pair[0]);
        if (removeEnd != null)
          ends.Remove(removeEnd);
        
        if (!nodes.ContainsKey(pair[1]))
        {
          var target = new GraphNode()
          {
            Index = pair[1],
            Words = vertices[Convert.ToInt32(pair[1])]
          };
          nodes.Add(pair[1], target);

          ends.Add(target);
        }

        if (nodes[pair[1]].Children == null)
          nodes[pair[1]].Children = new List<GraphNode>();

        nodes[pair[1]].Children.Add(source);
      }

      foreach (var node in ends)
      {
        root.Children.Add(node);
      }

      return root;
    }
  }
}