using System;
using System.Collections.Generic;
using Newtonsoft.Json;

namespace demo_site.Models
{
  public class DemoSimResult
  {
    [JsonProperty("sim")]
    public float Sim { get; set; }
    [JsonProperty("vertices")]
    public string[] Vertices { get; set; }
    [JsonProperty("edges")]
    public string[] Edges { get; set; }
    [JsonProperty("graph")]
    public GraphNode Graph { get;set; }
  }


  public class GraphNode
  {
    [JsonIgnore]
    public string Index { get; set; }
    [JsonProperty("name")]
    public string Words { get; set; }
    [JsonProperty(PropertyName = "children", NullValueHandling = NullValueHandling.Ignore)]
    public List<GraphNode> Children { get; set; }
  }
}