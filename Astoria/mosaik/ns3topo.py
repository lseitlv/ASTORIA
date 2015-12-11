import json
import networkx as nx
import xml.etree.ElementTree as ET


TOPO_FILE = 'data/ns3topo.xml'
TOPO_FILE2 = '../ns3/ns3topo.xml'

NODE_TYPES = ['PV','House','NODE','tr','MTU','RTU']


class Topo:

	def __init__(self,world,houses,gridFileName):
	
		self.sim_world = world
		self.houses = houses
		self.grid_filename = gridFileName
		self.house_active=True
	
		self.mosaik_graph= self.create_mosaik_graph()
		
	def create_mosaik_graph(self):
		Topo_ns3 = nx.Graph()
		with open(self.grid_filename) as data_file:
			grid_info = json.load(data_file)

		Topo_ns3.add_node("MTU");

		for mtuNodes in grid_info['mtu']:
			Topo_ns3.add_edge("MTU",mtuNodes, weight = 10)
		
		for node in grid_info['bus']:
			Topo_ns3.add_node(node[0])
		
		for branch in grid_info['branch']:
	    		Topo_ns3.add_edge(branch[1],branch[2],weight = 5) # definir valor de peso a partir da topo eletrica
		
		for prod in grid_info['prod']:
			Topo_ns3.add_edge(prod[0],prod[1],weight = 0)

		for prod in grid_info['trafo']:
			Topo_ns3.add_edge(prod[1],prod[2],weight = 0)
		
		house_data = self.sim_world.get_data(self.houses, 'node_id')
		
		if self.house_active:
			for house in self.houses:
				node_id = house_data[house]['node_id']
				Topo_ns3.add_edge(house.eid,node_id,weight = 0) #entre casas/placas e rede eletrica é zero?
		

		
		return Topo_ns3
			
	def create_p2p_topo_star(self):
	
		p2p_topo = nx.Graph()
		for node in self.mosaik_graph.nodes():
			p2p_topo.add_edge("MTU",node, weight = 10) #definir de alguma maneira (talvez relações dos caminhos pelos outros)
			
		return p2p_topo;

	def create_p2p_topo(self):
	
		graph = nx.Graph()
		graph = self.mosaik_graph;

		return graph;
	
	def get_node_type(self,nodeName):
		
		if("RTU" in nodeName):
			return "RTU"
		
		if("PV" in nodeName):
			return "PV"
		
		if("MTU" in nodeName):
			return "MTU"
		
		if("House" in nodeName):
			return "SENSOR"
		
		if("tr" in nodeName):
			return "tr"

		#for nodeType in NODE_TYPES:
		#	if(nodeType in nodeName):
		#		return nodeType
			
		return 'None'
		

	# edge[1] = name
	# edge[2] = weight
	def create_file(self,graph):
		
		root = ET.Element("topology")
		root.attrib['connection-type']= "P2P"	#TODO aqui vem as configs gerais da topologia
		for node in graph.nodes():
			nodeType = self.get_node_type(str(node))

			if nodeType is "MTU":
				xmlNode = ET.SubElement(root, "node",name=str(node),node_type=nodeType)

				for edge in graph.edges(node,data=True):
					edges = ET.SubElement(xmlNode,"edge",name = str(edge[1]))

					edges.attrib['weight'] = str(edge[2]['weight'])

		for node in graph.nodes():
			nodeType = self.get_node_type(str(node))

			if nodeType is not "MTU":
				xmlNode = ET.SubElement(root, "node",name=str(node),node_type=nodeType)

				for edge in graph.edges(node,data=True):
					edges = ET.SubElement(xmlNode,"edge",name = str(edge[1]))

					edges.attrib['weight'] = str(edge[2]['weight'])
			
		tree = ET.ElementTree(root)
		tree.write(TOPO_FILE)
		tree.write(TOPO_FILE2)
		
	


		
	
